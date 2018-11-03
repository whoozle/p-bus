#include <pbus/LocalBus.h>
#include <toolkit/net/unix/Endpoint.h>

namespace pbus
{
	LocalBus::LocalBus(const std::string &path): _socket(net::ISocket::NonBlocking)
	{
		net::unix::Endpoint ep(path);
		_socket.Listen(ep);
	}
}
