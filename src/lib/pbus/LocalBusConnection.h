#ifndef PBUS_LOCALBUSCONNECTION_H
#define PBUS_LOCALBUSCONNECTION_H

#include <pbus/ClassId.h>
#include <toolkit/io/DataStream.h>
#include <toolkit/io/IPollEventHandler.h>
#include <toolkit/io/Poll.h>
#include <toolkit/log/Logger.h>
#include <toolkit/net/unix/LocalSocket.h>
#include <deque>
#include <mutex>
#include <algorithm>

namespace pbus
{
	class LocalBus;
	class LocalBusConnection :
		public io::IPollEventHandler
	{
		static constexpr int DefaultEvents = io::Poll::EventInput | io::Poll::EventError | io::Poll::EventHangup;
		static constexpr size_t ReadBufferSize = 128 * 1024;

		std::mutex						_lock;
		ServiceId						_serviceId;
		log::Logger 					_log;
		io::Poll &						_poll;
		net::unix::LocalSocket			_socket;
		u32								_writeSerial = 0;
		u32								_readSerial = 0;

		struct WriteTask
		{
			ByteArray 	Data;
			size_t		Offset;

			WriteTask(ByteArray && data): Data(std::move(data)), Offset(0)
			{ }

			bool Finished() const
			{ return Offset >= Data.size(); }

			ConstBuffer GetBuffer()
			{ return ConstBuffer(Data, Offset); }

			void Complete(size_t size)
			{ Offset += size; }
		};

		struct ReadTask
		{
			std::array<u8, 4>	SizeBuffer;
			size_t				SizeBufferOffset;

			ByteArray			Data;
			size_t				DataOffset;

			ReadTask(): SizeBufferOffset(0), DataOffset(0)
			{ }
			bool Finished() const
			{ return SizeBufferOffset >= SizeBuffer.size() && DataOffset >= Data.size(); }

			size_t Read(ConstBuffer data)
			{
				size_t sizeRemains = std::min(SizeBuffer.size() - SizeBufferOffset, data.size());
				std::copy_n(data.begin(), sizeRemains, SizeBuffer.begin() + SizeBufferOffset);
				SizeBufferOffset += sizeRemains;
				if (SizeBufferOffset < SizeBuffer.size())
					return sizeRemains;

				if (sizeRemains != 0)
				{
					//size was just read
					Data.Resize(io::LittleEndianDataInputStream::ReadU32(SizeBuffer.data()));
					data = ConstBuffer(data, sizeRemains);
				}

				size_t dataRemains = std::min(Data.size() - DataOffset, data.size());
				std::copy_n(data.begin(), dataRemains, Data.begin() + DataOffset);
				DataOffset += dataRemains;
				return sizeRemains + dataRemains;
			}
		};

		std::deque<WriteTask> 	_writeQueue;
		ReadTask				_readTask;

	private:
		void Connect();
		void Reconnect();
		void EnableWrite(bool enable);
		void Wait(int timeout = -1);

	public:
		LocalBusConnection(ServiceId serviceId);
		LocalBusConnection(ServiceId serviceId, net::unix::LocalSocket && socket);
		~LocalBusConnection();

		net::unix::LocalSocket & GetSocket()
		{ return _socket; }

		void HandleSocketEvent(int event) override;

		u32 Send(ByteArray && data)
		{
			EnableWrite(true);
			std::lock_guard<decltype(_lock)> l(_lock);
			_writeQueue.emplace_back(std::move(data));
			return _writeSerial++;
		}
	};
}

#endif
