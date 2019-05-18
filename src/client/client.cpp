#include <pbus/LocalBusConnection.h>
#include <pbus/ServiceId.h>
#include <pbus/Session.h>
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
	auto random = session.GetService(ServiceId("random"));

	return 0;
}
