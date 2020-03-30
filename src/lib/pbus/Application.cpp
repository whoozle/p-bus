#include <pbus/Application.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <unistd.h>

namespace pbus
{
	Application::Application(const ServiceId & serviceId, int argc, char ** argv):
		_log("application/" + serviceId.ToString()),
		_bus(serviceId),
		_notifyParent(false)
	{
		{
			for(int i = 1; i < argc; ++i)
				if (strcmp(argv[i], "--notify-parent") == 0)
					_notifyParent = true;
		}
	}

	Application::~Application()
	{ }

	void Application::Run()
	{
		if (_notifyParent)
		{
			_log.Debug() << "notifying parent of startup";
			kill(getppid(), SIGUSR2);
		}
		auto & session = Session::Get();
		auto & poll = session.GetPoll();
		while(true)
			poll.Wait();
	}
}
