#ifndef PBUS_LOCALBUS_H
#define PBUS_LOCALBUS_H

#include <toolkit/net/unix/LocalServerSocket.h>
#include <string>

namespace pbus
{
	class ServiceRegistry;
	class LocalBus
	{
		ServiceRegistry &				_registry;
		net::unix::LocalServerSocket 	_socket;

	public:
		LocalBus(ServiceRegistry & registry, const std::string &path);
	};
}

#endif
