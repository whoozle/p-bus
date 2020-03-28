#include <pbus/Application.h>

namespace pbus
{
	Application::Application(const ServiceId & serviceId):
		_bus(serviceId)
	{ }

	void Application::Run()
	{
		auto & session = Session::Get();
		auto & poll = session.GetPoll();
		while(true)
			poll.Wait();
	}
}
