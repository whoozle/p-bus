#include <pbus/Application.h>

namespace pbus
{
	Application::Application(const ServiceId & serviceId):
		_bus(serviceId)
	{ }

	void Application::Run()
	{
		while(true)
			_bus.Wait();
	}
}
