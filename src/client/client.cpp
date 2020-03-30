#include <pbus/LocalBusConnection.h>
#include <pbus/ClassId.h>
#include <pbus/Session.h>
#include <pbus/Application.h>
#include <pbus/idl/system/IRandomGenerator.h>
#include <toolkit/log/Logger.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	using namespace pbus;
	Application app(ServiceId("app.pbus.example.random"), argc, argv);
	log::Logger logger("client");

	auto random = Session::Get().GetService<pbus::idl::system::IRandomGenerator>();
	for(int i = 0; i < 1000; ++i)
		logger.Info() << random->getInteger(100);

	return 0;
}
