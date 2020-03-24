#include <pbus/LocalBusConnection.h>
#include <pbus/ClassId.h>
#include <pbus/Session.h>
#include <pbus/idl/RandomGenerator.h>
#include <pbus/idl/ServiceManager.h>
#include <toolkit/log/Logger.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	using namespace pbus;
	log::Logger logger("client");

	pbus::idl::RandomGenerator::Register();
	pbus::idl::ServiceManager::Register();

	auto random = Session::Get().GetService<pbus::idl::RandomGenerator>(ClassId("RandomGenerator"));
	for(int i = 0; i < 1000; ++i)
		logger.Info() << random->getInteger(100);

	return 0;
}
