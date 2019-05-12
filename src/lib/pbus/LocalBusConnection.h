#ifndef PBUS_LOCALBUSCONNECTION_H
#define PBUS_LOCALBUSCONNECTION_H

#include <pbus/ServiceId.h>
#include <toolkit/log/Logger.h>
#include <toolkit/net/unix/LocalSocket.h>

namespace pbus
{
	class LocalBusConnection
	{
		log::Logger 					_log;
		net::unix::LocalSocket			_socket;

	public:
		LocalBusConnection(ServiceId serviceId);
		LocalBusConnection(ServiceId serviceId, net::unix::LocalSocket && socket);
	};
}

#endif
