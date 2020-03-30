#include <pbus/Application.h>
#include <system/servicemanager/ServiceManager.h>

int main(int argc, char **argv)
{
	using namespace pbus;
	Application app(system::servicemanager::Manager::ClassId, argc, argv);
	app.RegisterService<system::servicemanager::Manager>();
	app.Run();
	return 0;
}
