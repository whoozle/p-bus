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
		_poll.Add(_socket, _accept, io::Poll::EventInput | io::Poll::EventError);
	}

	void LocalBus::Accept()
	{
		_log.Debug() << "accepting connection...";
		std::unique_ptr<net::unix::LocalSocket> sock(_socket.Accept());
		if (!sock)
			return;

		auto cred = sock->GetPeerCredentials();
		_log.Debug() << "credentials, pid: " << cred.pid << ", gid: " << cred.gid << ", uid: " << cred.uid;

		new LocalBusConnection(_id, std::move(*sock));
	}

}
