#include <pbus/servicemanager/Service.h>
#include <pbus/servicemanager/ServiceRegistry.h>
#include <pbus/LocalBus.h>

int main(int argc, char **argv)
{
	using namespace pbus;
	ServiceId serviceId("init");
	ServiceRegistry registry;
	ServiceDescriptor initDescriptor;
	initDescriptor.Executable = argv[0];
	registry.Add(serviceId, std::move(initDescriptor));
	LocalBus bus(serviceId);
	return 0;
}
