#include <pbus/LocalBus.h>
#include <toolkit/net/unix/Endpoint.h>
#include <unistd.h>

namespace pbus
{
	LocalBus::LocalBus(const ServiceId & id):
		_log("bus/" + id.ToString()),
		_socket(net::ISocket::NonBlocking)
	{
		const std::string path = id.ToString();
		_log.Debug() << "creating socket at " << path;
		net::unix::Endpoint ep(path);
		unlink(path.c_str());
		_socket.Listen(ep);
		_poll.Add(_socket, _accept, io::Poll::EventInput);
	}

	void LocalBus::Accept()
	{
		_log.Debug() << "accepting connection...";
	}

	void LocalBus::Wait(int timeout)
	{
		_poll.Wait(timeout);
	}
}
