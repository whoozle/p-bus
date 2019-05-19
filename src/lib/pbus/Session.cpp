#include <pbus/Session.h>

namespace pbus
{
	log::Logger Session::_log("session");

	LocalBusConnectionPtr Session::Connect(const ServiceId & serviceId)
	{
		auto i = _connections.find(serviceId);
		if (i == _connections.end())
		{
			try
			{
				_connections.insert(std::make_pair(serviceId, std::make_shared<LocalBusConnection>(serviceId)));
			}
			catch(const std::exception & ex)
			{
				_log.Debug() << "exception while connection: " << ex.what();
			}
		}
	}

}
