#ifndef PBUS_LOCALBUS_H
#define PBUS_LOCALBUS_H

#include <pbus/ClassId.h>
#include <toolkit/net/unix/LocalServerSocket.h>
#include <toolkit/io/IPollEventHandler.h>
#include <toolkit/io/Poll.h>
#include <toolkit/log/Logger.h>
#include <string>

namespace pbus
{
	class LocalBusConnection;
	class LocalBus
	{
	private:
		log::Logger 					_log;
		ClassId							_id;
		net::unix::LocalServerSocket 	_socket;
		io::Poll &						_poll;

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
		friend class LocalBusConnection;
		void Accept();


	public:
		LocalBus(const ClassId &id);
	};
}

#endif
