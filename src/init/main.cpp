#include <pbus/servicemanager/Service.h>
#include <pbus/servicemanager/ServiceRegistry.h>

int main(int argc, char **argv)
{
	using namespace pbus;
	ServiceRegistry registry;
	ServiceDescriptor initDescriptor;
	initDescriptor.Executable = argv[0];
	registry.Add(ServiceId("init"), std::move(initDescriptor));
	return 0;
}
