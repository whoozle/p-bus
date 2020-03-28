#ifndef PBUS_LOCALBUSCONNECTION_H
#define PBUS_LOCALBUSCONNECTION_H

#include <pbus/ClassId.h>
#include <toolkit/io/IPollEventHandler.h>
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
		std::mutex						_lock;
		log::Logger 					_log;
		LocalBus *						_bus;
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

			Buffer ReadBuffer()
			{ return Buffer(Data, Offset); }
		};

		std::deque<Task> _writeQueue, _readQueue;

	private:
		void EnableWrite(bool enable);

	public:
		LocalBusConnection(ClassId serviceId);
		LocalBusConnection(ClassId serviceId, LocalBus * bus, net::unix::LocalSocket && socket);
		~LocalBusConnection();

		net::unix::LocalSocket & GetSocket()
		{ return _socket; }

		void HandleSocketEvent(int event) override;

		u32 Send(ByteArray && data)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			_writeQueue.emplace_back(std::move(data));
			return _serial++;
		}
	};
}

#endif
