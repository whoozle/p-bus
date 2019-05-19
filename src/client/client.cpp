#include <pbus/LocalBusConnection.h>
#include <pbus/ServiceId.h>
#include <pbus/Session.h>
#include <pbus/idl/RandomGenerator.h>
#include <toolkit/log/Logger.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	using namespace pbus;
	log::Logger logger("client");

	SessionPtr session = std::make_shared<Session>();
	auto random = session->GetService<pbus::idl::RandomGenerator>(ServiceId("RandomGenerator"));
	for(int i = 0; i < 1000; ++i)
		logger.Info() << random->getInteger(100);

	return 0;
}
