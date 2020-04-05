#include <pbus/LocalBusConnection.h>
#include <pbus/LocalBus.h>
#include <pbus/Session.h>
#include <toolkit/net/unix/Endpoint.h>
#include <toolkit/text/Formatters.h>
#include <pbus/idl/system/IServiceManager.h>

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
	{ }

	LocalBusConnection::LocalBusConnection(ServiceId serviceId, net::unix::LocalSocket && socket):
		_serviceId(serviceId),
		_log("connection/" + serviceId.ToString()),
		_poll(Session::Get().GetPoll()),
		_socket(std::make_shared<Socket>(std::move(socket)))
	{
		_poll.Add(*_socket, *this, DefaultEvents);
	}

	std::string LocalBusConnection::GetSocketPath(const ServiceId & serviceId)
	{
		text::StringOutputStream ss;
#ifdef PBUS_DEVEL_MODE
		ss << ".";
#endif
		ss << "/packages/" << serviceId << "/socket";

		return ss.Get();
	}

	void LocalBusConnection::Connect()
	{
		if (_socket)
			return;

		_log.Debug() << "connecting to " << _serviceId;
		_socket = std::make_shared<Socket>();

		net::unix::Endpoint ep(GetSocketPath(_serviceId), false);
		try
		{ _socket->Connect(ep); }
		catch (std::exception & ex)
		{
			_log.Debug() << "exception while connecting to " << _serviceId << ": " << ex;
			if (_serviceId == idl::system::IServiceManager::ClassId)
				throw Exception("cannot connect to ServiceManager to create service " + _serviceId.ToString());

			_log.Debug() << "connecting to service manager at " << idl::system::IServiceManager::ClassId;
			Session & session = Session::Get();
			auto serviceManager = session.GetService<idl::system::IServiceManager>();
			/*auto lease =*/ serviceManager->start(_serviceId.Name, _serviceId.Version);
			_socket->Connect(ep);
		}
		_socket->SetNonBlocking(true);
		_poll.Add(*_socket, *this, DefaultEvents);
	}

	LocalBusConnection::~LocalBusConnection()
	{
		_log.Debug() << "closing...";
		try { if (_socket) _poll.Remove(*_socket);  } catch (const std::exception & ex) { _log.Warning() << "failure in connection dtor: " << ex; }
	}

	void LocalBusConnection::EnableWrite(bool enable)
	{ _poll.Modify(*_socket, *this, DefaultEvents | (enable? _poll.EventOutput: 0)); }

	void LocalBusConnection::HandleSocketEvent(int event)
	{
		_log.Trace() << "socket event 0x" << text::Hex(event);
		if (event & (io::Poll::EventHangup | io::Poll::EventError))
		{
			_log.Warning() << "error or hangup, reconnect";
			_poll.Remove(*_socket);
			_socket.reset();
			return;
		}

		if (event & io::Poll::EventOutput)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			if (!_writeQueue.empty())
			{
				auto & task = _writeQueue.front();
				auto buffer = task.GetBuffer();
				auto r = _socket->Write(buffer);
				_log.Trace() << "wrote " << r << " bytes";
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
			auto r = _socket->Read(buffer);
			_log.Trace() << "read " << r << " bytes";
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
