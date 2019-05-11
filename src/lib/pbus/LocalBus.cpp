#include <pbus/LocalBus.h>
#include <toolkit/net/unix/Endpoint.h>

namespace pbus
{
	LocalBus::LocalBus(ServiceRegistry & registry, const std::string &path):
		_registry(registry), _socket(net::ISocket::NonBlocking)
	{
		net::unix::Endpoint ep(path);
		_socket.Listen(ep);
	}
}
