#ifndef PBUS_LOCALBUS_H
#define PBUS_LOCALBUS_H

#include <pbus/ServiceId.h>
#include <toolkit/net/unix/LocalServerSocket.h>
#include <toolkit/log/Logger.h>
#include <string>

namespace pbus
{
	class LocalBus
	{
		log::Logger 					_log;
		net::unix::LocalServerSocket 	_socket;

	public:
		LocalBus(const ServiceId &id);
	};
}

#endif
