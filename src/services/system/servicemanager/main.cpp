#include <pbus/Application.h>

int main(int argc, char **argv)
{
	using namespace pbus;
	Application app(ClassId("system.ServiceManager"));
	app.Run();
	return 0;
}
