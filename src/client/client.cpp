#include <pbus/LocalBusConnection.h>
#include <pbus/ServiceId.h>
#include <toolkit/log/Logger.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "usage: <service> <method>\n");
		return 0;
	}
	using namespace pbus;
	log::Logger logger("client");

	std::string serviceName;
	ServiceId serviceId(argv[1]);
	logger.Debug() << "connecting to " << serviceId;
	return 0;
}
