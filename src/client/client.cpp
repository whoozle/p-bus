#include <pbus/LocalBusConnection.h>
#include <pbus/ServiceId.h>
#include <pbus/Session.h>
#include <pbus/idl/IRandomGenerator.h>
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

	Session session;
	auto random = session.GetService<pbus::idl::IRandomGenerator>(ServiceId("RandomGenerator"));
	for(int i = 0; i < 1000; ++i)
		logger.Info() << random->getInteger(100);

	return 0;
}
