#include <pbus/Application.h>

namespace pbus
{
	Application::Application(const ClassId & classId):
		_bus(classId)
	{ }

	void Application::Run()
	{
		_bus.Wait();
	}
}
