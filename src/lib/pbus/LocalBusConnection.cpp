#include <pbus/LocalBusConnection.h>
#include <pbus/LocalBus.h>
#include <toolkit/net/unix/Endpoint.h>
#include <toolkit/text/Formatters.h>

namespace pbus
{
	LocalBusConnection::LocalBusConnection(ClassId serviceId):
		_log("connection/" + serviceId.ToString()),
		_bus(nullptr),
		_socket(net::BaseSocket::NonBlocking)
	{
		auto path = serviceId.ToString();
		_log.Debug() << "connecting to " << serviceId;
		net::unix::Endpoint ep(path);
		_socket.Connect(ep);
	}

	LocalBusConnection::LocalBusConnection(ClassId serviceId, LocalBus * bus, net::unix::LocalSocket && socket):
		_log("connection/" + serviceId.ToString()),
		_bus(bus),
		_socket(std::move(socket))
	{ _bus->Add(this); }

	LocalBusConnection::~LocalBusConnection()
	{ if (_bus) _bus->Remove(this); }

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
