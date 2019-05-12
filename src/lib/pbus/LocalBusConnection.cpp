#include <pbus/LocalBusConnection.h>
#include <toolkit/net/unix/Endpoint.h>

namespace pbus
{
	LocalBusConnection::LocalBusConnection(ServiceId serviceId):
		_log("connection/" + serviceId.ToString()),
		_socket(net::BaseSocket::NonBlocking)
	{
		auto path = serviceId.ToString();
		_log.Debug() << "connecting to " << serviceId;
		net::unix::Endpoint ep(path);
		_socket.Connect(ep);
	}
}
