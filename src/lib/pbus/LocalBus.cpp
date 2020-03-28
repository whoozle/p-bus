#include <pbus/LocalBus.h>
#include <pbus/LocalBusConnection.h>
#include <pbus/Session.h>
#include <toolkit/net/unix/Endpoint.h>
#include <unistd.h>

namespace pbus
{
	LocalBus::LocalBus(const ClassId & id):
		_log("bus/" + id.ToString()),
		_id(id),
		_poll(Session::Get().GetPoll()),
		_accept(this)
	{
		_socket.SetNonBlocking(true);
		auto path = id.ToString();
		_log.Debug() << "creating socket at " << path;
		net::unix::Endpoint ep(path);
		//unlink(path.c_str());
		_socket.Listen(ep);
		_poll.Add(_socket, _accept, DefaultEvents);
	}

	void LocalBus::Accept()
	{
		_log.Debug() << "accepting connection...";
		std::unique_ptr<net::unix::LocalSocket> sock(_socket.Accept());
		if (!sock)
			return;

		auto cred = sock->GetPeerCredentials();
		_log.Debug() << "credentials, pid: " << cred.pid << ", gid: " << cred.gid << ", uid: " << cred.uid;

		new LocalBusConnection(_id, this, std::move(*sock));
	}

	void LocalBus::Add(LocalBusConnection * connection)
	{
		_poll.Add(connection->GetSocket(), *connection, DefaultEvents);
	}

	void LocalBus::Remove(LocalBusConnection * connection)
	{
		_poll.Remove(connection->GetSocket());
	}

	void LocalBus::EnableWrite(LocalBusConnection * connection, bool enable)
	{
		_poll.Modify(connection->GetSocket(), *connection, DefaultEvents | (enable? _poll.EventOutput: 0));
	}

	void LocalBus::Wait(int timeout)
	{
		_poll.Wait(timeout);
	}
}
