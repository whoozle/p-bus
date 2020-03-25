#include <pbus/ClassId.h>
#include <pbus/LocalBus.h>

int main(int argc, char **argv)
{
	using namespace pbus;
	ClassId serviceId("ServiceManager");
	LocalBus bus(serviceId);
	while(true)
		bus.Wait();
	return 0;
}
