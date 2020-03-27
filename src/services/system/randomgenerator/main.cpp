#include <pbus/Application.h>

int main(int argc, char **argv)
{
	using namespace pbus;
	Application app(ServiceId("system.RandomGenerator"));
	app.Run();
	return 0;
}
