#ifndef PBUS_LOCALBUS_H
#define PBUS_LOCALBUS_H

#include <pbus/ServiceId.h>
#include <toolkit/net/unix/LocalServerSocket.h>
#include <toolkit/io/IPollEventHandler.h>
#include <toolkit/io/Poll.h>
#include <toolkit/log/Logger.h>
#include <string>

namespace pbus
{
	class LocalBus
	{
	private:
		log::Logger 					_log;
		ServiceId						_id;
		net::unix::LocalServerSocket 	_socket;
		io::Poll						_poll;

		struct ConnectionAcceptor : public io::IPollEventHandler
		{
			LocalBus * Bus;
			ConnectionAcceptor(LocalBus * bus) : Bus(bus)
			{ }
			void HandleSocketEvent(int event) override
			{ Bus->Accept(); }
		};
		ConnectionAcceptor				_accept;

	private:
		void Accept();

	public:
		LocalBus(const ServiceId &id);

		void Wait(int timeout = -1);
	};
}

#endif
