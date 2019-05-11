#include <pbus/servicemanager/ServiceRegistry.h>
namespace pbus
{

	log::Logger	ServiceRegistry::_log("service-registry");

	void ServiceRegistry::Add(const ServiceId & id, ServiceDescriptor && desc)
	{
		_log.Info() << "registering service " << id;
		_services.emplace(std::make_pair(id, std::move(desc)));
	}

	const ServiceDescriptor * ServiceRegistry::GetDescriptor(const ServiceId & id) const
	{
		auto it = _services.find(id);
		return it != _services.end()? &it->second: nullptr;
	}

}
