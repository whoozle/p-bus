#ifndef PBUS_LOCALBUS_H
#define PBUS_LOCALBUS_H

#include <toolkit/net/unix/LocalServerSocket.h>
#include <string>

namespace pbus
{
	class LocalBus
	{
		net::unix::LocalServerSocket _socket;

	public:
		LocalBus(const std::string &path);
	};
}

#endif
