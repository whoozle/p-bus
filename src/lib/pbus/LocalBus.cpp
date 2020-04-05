#include <pbus/LocalBus.h>
#include <pbus/LocalBusConnection.h>
#include <pbus/Session.h>
#include <toolkit/net/unix/Endpoint.h>
#include <toolkit/io/File.h>
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
		auto path = LocalBusConnection::GetSocketPath(id);
		unlink(path.c_str());
		_log.Debug() << "creating socket at " << path;
		net::unix::Endpoint ep(path, false);
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

		//this is only dev mode hack
		auto imagePath = io::File::ReadLink("/proc/" + std::to_string(cred.pid) + "/exe");
		_log.Debug() << "image path: " << imagePath;

		std::string serviceIdString;
		{
			auto slashPos = imagePath.rfind('/');
			if (slashPos == imagePath.npos)
				throw Exception("invalid image path");
			serviceIdString = imagePath.substr(slashPos + 1);
		}

		auto serviceId = ServiceId::FromString(serviceIdString);
		_log.Debug() << "parsed service id from /proc: " << serviceId;

		auto connection = std::make_shared<LocalBusConnection>(serviceId, std::move(*sock));
		Session::Get().AddConnection(serviceId, connection);
	}

}
