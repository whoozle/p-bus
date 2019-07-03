#ifndef PBUS_LOCALBUSCONNECTION_H
#define PBUS_LOCALBUSCONNECTION_H

#include <pbus/ServiceId.h>
#include <toolkit/io/IPollEventHandler.h>
#include <toolkit/log/Logger.h>
#include <toolkit/net/unix/LocalSocket.h>

namespace pbus
{
	class LocalBus;
	class LocalBusConnection :
		public io::IPollEventHandler
	{
		log::Logger 					_log;
		LocalBus *						_bus;
		net::unix::LocalSocket			_socket;

	public:
		LocalBusConnection(ServiceId serviceId);
		LocalBusConnection(ServiceId serviceId, LocalBus * bus, net::unix::LocalSocket && socket);
		~LocalBusConnection();

		net::unix::LocalSocket & GetSocket()
		{ return _socket; }
		void HandleSocketEvent(int event) override;
	};
}

#endif
