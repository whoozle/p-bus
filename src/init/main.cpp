#include <pbus/ServiceId.h>
#include <pbus/LocalBus.h>

int main(int argc, char **argv)
{
	using namespace pbus;
	ServiceId serviceId("ServiceManager");
	LocalBus bus(serviceId);
	while(true)
		bus.Wait();
	return 0;
}
