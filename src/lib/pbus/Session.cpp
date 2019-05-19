#include <pbus/Session.h>
#include <pbus/idl/IServiceManager.h>

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
				auto connection = std::make_shared<LocalBusConnection>(serviceId);
				_connections.insert(std::make_pair(serviceId, connection));
				return connection;
			}
			catch(const std::exception & ex)
			{
				_log.Debug() << "exception while connection: " << ex.what();
				ServiceId serviceManagerId("ServiceManager");
				if (serviceId == serviceManagerId)
					throw Exception("cannot connect to ServiceManager to create service " + serviceId.ToString());
				_log.Debug() << "connecting to " << serviceManagerId;
				auto serviceManager = GetService<idl::IServiceManager>(serviceManagerId);
				auto lease = serviceManager->initialize(serviceId.Name, serviceId.Version);
				auto connection = std::make_shared<LocalBusConnection>(serviceId);
				_connections.insert(std::make_pair(serviceId, connection));
				return connection;
			}
		}
		else
			return i->second;
	}

}
