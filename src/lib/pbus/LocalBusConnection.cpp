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
		_log("connection/" + serviceId.ToString()),
		_poll(Session::Get().GetPoll())
	{
		_socket.SetNonBlocking(true);
		auto path = serviceId.ToString();
		_log.Debug() << "connecting to " << serviceId;
		net::unix::Endpoint ep(path);
		_socket.Connect(ep);
		_poll.Add(_socket, *this, DefaultEvents);
	}

	LocalBusConnection::LocalBusConnection(ClassId serviceId, net::unix::LocalSocket && socket):
		_log("connection/" + serviceId.ToString()),
		_poll(Session::Get().GetPoll()),
		_socket(std::move(socket))
	{
		_poll.Add(_socket, *this, DefaultEvents);
	}

	LocalBusConnection::~LocalBusConnection()
	{
		_log.Debug() << "closing...";
		Session::Get().GetPoll().Remove(_socket);
	}

	void LocalBusConnection::EnableWrite(bool enable)
	{ _poll.Modify(_socket, *this, DefaultEvents | (enable? _poll.EventOutput: 0)); }

	void LocalBusConnection::HandleSocketEvent(int event)
	{
		_log.Debug() << "socket event 0x" << text::Hex(event);
		if (event & (io::Poll::EventHangup | io::Poll::EventError))
		{
			_log.Debug() << "error or hangup, cleaning up...";
			delete this;
			return;
		}
	}

}
