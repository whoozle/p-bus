#include <pbus/Application.h>
#include <toolkit/io/File.h>
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

			int pid = getpid();
			//fixme: dev mode
			mkdir("/var/run/pbus", 0777);

			char buf[1024];
			snprintf(buf, sizeof(buf), "/var/run/pbus/%d", (int)pid);
			io::File f(buf, io::FileOpenMode::Overwrite);

			char body[1024];
			auto r = snprintf(body, sizeof(body), "%s@%d", serviceId.Name.c_str(), serviceId.Version);
			f.Write(ConstBuffer(reinterpret_cast<const u8 *>(body), r));
		}
	}

	Application::~Application()
	{
		{
			int pid = getpid();
			char buf[1024];
			snprintf(buf, sizeof(buf), "/var/run/pbus/%d", (int)pid);
			unlink(buf);
		}

	}

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
