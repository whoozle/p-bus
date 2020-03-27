#include <pbus/Session.h>
#include <pbus/idl/system/IServiceManager.h>

namespace pbus
{
	log::Logger Session::_log("session");

	Session::Session()
	{
		idl::system::IServiceManager::RegisterProxy(*this);
	}

	LocalBusConnectionPtr Session::Connect(const ServiceId & serviceId)
	{
		_log.Debug() << "connecting to " << serviceId;
		auto i = _connections.find(serviceId);
		if (i != _connections.end())
			return i->second;

		try
		{
			auto connection = std::make_shared<LocalBusConnection>(serviceId);
			_connections.insert(std::make_pair(serviceId, connection));
			return connection;
		}
		catch(const std::exception & ex)
		{
			_log.Debug() << "exception while connection: " << ex.what();
			if (serviceId == idl::system::IServiceManager::ClassId)
				throw Exception("cannot connect to ServiceManager to create service " + serviceId.ToString());

			_log.Debug() << "connecting to service manager at " << idl::system::IServiceManager::ClassId;
			auto serviceManager = GetService<idl::system::IServiceManager>();
			auto lease = serviceManager->start(serviceId.Name, serviceId.Version);
			auto connection = std::make_shared<LocalBusConnection>(serviceId);
			_connections.insert(std::make_pair(serviceId, connection));
			return connection;
		}
	}

}
