#include <pbus/ServiceRegistry.h>
namespace pbus
{

	log::Logger	ServiceRegistry::_log("service-registry");

	void ServiceRegistry::Add(ServiceId id, ServiceDescriptor && desc)
	{
		_services.emplace(std::make_pair(id, std::move(desc)));
	}

}
