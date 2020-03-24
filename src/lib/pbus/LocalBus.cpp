#include <pbus/LocalBus.h>
#include <pbus/LocalBusConnection.h>
#include <toolkit/net/unix/Endpoint.h>
#include <unistd.h>

namespace pbus
{
	LocalBus::LocalBus(const ClassId & id):
		_log("bus/" + id.ToString()),
		_id(id),
		_socket(net::ISocket::NonBlocking),
		_accept(this)
	{
		auto path = id.ToString();
		_log.Debug() << "creating socket at " << path;
		net::unix::Endpoint ep(path);
		//unlink(path.c_str());
		_socket.Listen(ep);
		_poll.Add(_socket, _accept, io::Poll::EventInput);
	}

	void LocalBus::Accept()
	{
		_log.Debug() << "accepting connection...";
		auto sock = _socket.Accept();
		if (!sock)
			return;

		auto cred = sock->GetPeerCredentials();
		_log.Debug() << "credentials, pid: " << cred.pid << ", gid: " << cred.gid << ", uid: " << cred.uid;

		auto localConnection = new LocalBusConnection(_id, this, std::move(*sock));
		delete sock;
	}

	void LocalBus::Add(LocalBusConnection * connection)
	{
		_poll.Add(connection->GetSocket(), *connection, _poll.EventInput);
	}

	void LocalBus::Remove(LocalBusConnection * connection)
	{
		_poll.Remove(connection->GetSocket());
	}

	void LocalBus::AllowWrite(LocalBusConnection * connection, bool allow)
	{
		_poll.Modify(connection->GetSocket(), *connection, _poll.EventInput | (allow? _poll.EventOutput: 0));
	}

	void LocalBus::Wait(int timeout)
	{
		_poll.Wait(timeout);
	}
}
