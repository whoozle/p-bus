#include <pbus/Service.h>
#include <pbus/LocalBus.h>
#include <pbus/ServiceRegistry.h>

int main(int argc, char **argv)
{
	using namespace pbus;
	ServiceRegistry registry;
	ServiceDescriptor initDescriptor;
	initDescriptor.Executable = argv[0];
	registry.Add(ServiceId("init"), std::move(initDescriptor));
	return 0;
}
