#include <pbus/Session.h>
#include <pbus/idl/system/ServiceManager.h>

namespace pbus
{
	log::Logger Session::_log("session");

	Session::Session()
	{ }

	LocalBusConnectionPtr Session::Connect(const ClassId & serviceId)
	{
		_log.Debug() << "connecting to " << serviceId;
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
				if (serviceId == idl::system::ServiceManager::ClassId)
					throw Exception("cannot connect to ServiceManager to create service " + serviceId.ToString());

				_log.Debug() << "connecting to service manager at " << idl::system::ServiceManager::ClassId;
				auto serviceManager = GetService<idl::system::ServiceManager>();
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
