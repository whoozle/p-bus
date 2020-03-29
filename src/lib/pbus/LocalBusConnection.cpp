#include <pbus/LocalBusConnection.h>
#include <pbus/LocalBus.h>
#include <pbus/Session.h>
#include <toolkit/net/unix/Endpoint.h>
#include <toolkit/text/Formatters.h>

namespace pbus
{
	// void LocalBus::EnableWrite(LocalBusConnection * connection, bool enable)
	// {
	// 	_poll.Modify(connection->GetSocket(), *connection, DefaultEvents | (enable? _poll.EventOutput: 0));
	// }



	LocalBusConnection::LocalBusConnection(ClassId serviceId):
		_serviceId(serviceId),
		_log("connection/" + serviceId.ToString()),
		_poll(Session::Get().GetPoll())
	{
		_socket.SetNonBlocking(true);
		_log.Debug() << "connecting to " << serviceId;
		Connect();
	}

	LocalBusConnection::LocalBusConnection(ServiceId serviceId, net::unix::LocalSocket && socket):
		_serviceId(serviceId),
		_log("connection/" + serviceId.ToString()),
		_poll(Session::Get().GetPoll()),
		_socket(std::move(socket))
	{
		_poll.Add(_socket, *this, DefaultEvents);
	}

	void LocalBusConnection::Reconnect()
	{
		_socket.~LocalSocket();
		new (&_socket) net::unix::LocalSocket();
		Connect();
	}

	void LocalBusConnection::Connect()
	{
		net::unix::Endpoint ep(_serviceId.ToString());
		_socket.Connect(ep);
		_poll.Add(_socket, *this, DefaultEvents);
	}

	LocalBusConnection::~LocalBusConnection()
	{
		_log.Debug() << "closing...";
		_poll.Remove(_socket);
	}

	void LocalBusConnection::EnableWrite(bool enable)
	{ _poll.Modify(_socket, *this, DefaultEvents | (enable? _poll.EventOutput: 0)); }

	void LocalBusConnection::HandleSocketEvent(int event)
	{
		_log.Debug() << "socket event 0x" << text::Hex(event);
		if (event & (io::Poll::EventHangup | io::Poll::EventError))
		{
			_log.Warning() << "error or hangup, reconnect";
			Reconnect();
			return;
		}

		if (event & io::Poll::EventOutput)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			if (!_writeQueue.empty())
			{
				auto & task = _writeQueue.front();
				auto buffer = task.GetBuffer();
				auto r = _socket.Write(buffer);
				_log.Debug() << "wrote " << r << " bytes";
				task.Complete(r);
				if (task.Finished())
					_writeQueue.pop_front();
				if (_writeQueue.empty())
					EnableWrite(false);
			}
			else
				EnableWrite(false);
		}

		if (event & io::Poll::EventInput)
		{
			std::unique_lock<decltype(_lock)> l(_lock);
			ByteArray buffer(ReadBufferSize);
			auto r = _socket.Read(buffer);
			_log.Debug() << "read " << r << " bytes";
			if (r <= 0)
				throw Exception("socket read returned bogus value, shutdown?");

			Buffer data(buffer, 0, r);
			size_t offset = 0;
			while(offset < data.size())
			{
				offset += _readTask.Read(Buffer(data, offset));
				if (_readTask.Finished()) {
					try
					{
						auto serial = _readSerial++;
						l.unlock();
						Session::Get().OnIncomingData(_serviceId, serial, _readTask.Data);
						l.lock();
					}
					catch(const std::exception & ex)
					{ _log.Error() << "incoming data parsing failure: " << ex; }
					_readTask = ReadTask();
				}
			}
		}
	}

}
