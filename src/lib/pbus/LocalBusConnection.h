#ifndef PBUS_LOCALBUSCONNECTION_H
#define PBUS_LOCALBUSCONNECTION_H

#include <pbus/ClassId.h>
#include <toolkit/io/IPollEventHandler.h>
#include <toolkit/io/Poll.h>
#include <toolkit/log/Logger.h>
#include <toolkit/net/unix/LocalSocket.h>
#include <deque>
#include <mutex>

namespace pbus
{
	class LocalBus;
	class LocalBusConnection :
		public io::IPollEventHandler
	{
		static constexpr int DefaultEvents = io::Poll::EventInput | io::Poll::EventError | io::Poll::EventHangup;

		std::mutex						_lock;
		log::Logger 					_log;
		io::Poll &						_poll;
		net::unix::LocalSocket			_socket;
		u32								_serial;

		struct Task
		{
			ByteArray 	Data;
			size_t		Offset;

			Task(ByteArray && data): Data(std::move(data)), Offset(0)
			{ }

			bool Finished() const
			{ return Offset >= Data.size(); }

			Buffer GetBuffer()
			{ return Buffer(Data, Offset); }

			void Complete(size_t size)
			{ Offset += size; }
		};

		std::deque<Task> _writeQueue, _readQueue;

	private:
		void EnableWrite(bool enable);
		void Wait(int timeout = -1);

	public:
		LocalBusConnection(ClassId serviceId);
		LocalBusConnection(ClassId serviceId, net::unix::LocalSocket && socket);
		~LocalBusConnection();

		net::unix::LocalSocket & GetSocket()
		{ return _socket; }

		void HandleSocketEvent(int event) override;

		u32 Send(ByteArray && data)
		{
			EnableWrite(true);
			std::lock_guard<decltype(_lock)> l(_lock);
			_writeQueue.emplace_back(std::move(data));
			return _serial++;
		}
	};
}

#endif
