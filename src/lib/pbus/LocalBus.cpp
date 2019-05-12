#include <pbus/LocalBus.h>
#include <pbus/LocalBusConnection.h>
#include <toolkit/net/unix/Endpoint.h>
#include <unistd.h>

namespace pbus
{
	LocalBus::LocalBus(const ServiceId & id):
		_log("bus/" + id.ToString()),
		_id(id),
		_socket(net::ISocket::NonBlocking),
		_accept(this)
	{
		auto path = id.ToString();
		_log.Debug() << "creating socket at " << path;
		net::unix::Endpoint ep(path);
		unlink(path.c_str());
		_socket.Listen(ep);
		_poll.Add(_socket, _accept, io::Poll::EventInput);
	}

	void LocalBus::Accept()
	{
		_log.Debug() << "accepting connection...";
		auto sock = _socket.Accept();
		if (!sock)
			return;

		auto localConnection = new LocalBusConnection(_id, this, std::move(*sock));
		delete sock;
	}

	void LocalBus::Add(LocalBusConnection * connection)
	{
		_poll.Add(*connection, *connection, _poll.EventInput);
	}

	void LocalBus::Remove(LocalBusConnection * connection)
	{
		_poll.Remove(*connection);
	}

	void LocalBus::AllowWrite(LocalBusConnection * connection, bool allow)
	{
		_poll.Modify(*connection, *connection, _poll.EventInput | (allow? _poll.EventOutput: 0));
	}

	void LocalBus::Wait(int timeout)
	{
		_poll.Wait(timeout);
	}
}
