#include <system/servicemanager/ServiceManager.h>
#include <toolkit/io/File.h>
#include <toolkit/io/SystemException.h>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/signal.h>

namespace pbus { namespace system { namespace servicemanager
{
	namespace
	{
		std::atomic<bool> g_started;

		void g_OnSigUSR2(int signo)
		{ g_started = true; }
	}
	log::Logger Manager::_log("ServiceManagerImpl");

	Manager::Manager()
	{
		std::string currentExe = io::File::ReadLink("/proc/self/exe");
		auto slashPos = currentExe.rfind('/');
		if (slashPos == currentExe.npos)
			throw Exception("could not find root directory");

		_root = currentExe.substr(0, slashPos);
		_log.Debug() << "root = " << _root;
		_session = setsid();
		if (_session == -1)
			_log.Warning() << "setsid failed: " << io::SystemException::GetErrorMessage();

		struct sigaction act = {};
		act.sa_handler = SIG_IGN;
		if (sigaction(SIGCHLD, &act, NULL) == -1)
			_log.Warning() << "failed to install SIGCHLD handler: " << io::SystemException::GetErrorMessage();

		act.sa_handler = &g_OnSigUSR2;
		if (sigaction(SIGUSR2, &act, NULL) == -1)
			_log.Warning() << "failed to install SIGUSR2 handler: " << io::SystemException::GetErrorMessage();
	}

	int Manager::RunProcessTrampoline(void * arg)
	{
		Process * p = static_cast<Process *>(arg);
		return p->Self->RunProcess(*p);
	}

	int Manager::RunProcess(const Process & process)
	{
		_log.Info() << "NEW PROCESS";
		if (execl(process.Path.c_str(), process.Path.c_str(), "--notify-parent", NULL) == -1) {
			_log.Error() << "exec failed: " << io::SystemException::GetErrorMessage();
		}
		exit(1);
		return 1;
	}

	std::string Manager::GetServicePath(const ServiceId & serviceId)
	{
		text::StringOutputStream os;
		os << _root << "/" << serviceId.Name; //fixme: no version here right now
		return os.Get();
	}

	void Manager::start(const std::string & name, u16 version)
	{
		ServiceId serviceId(name, version);

		g_started = false;

		Process process;
		process.Self = this;
		process.Path = GetServicePath(serviceId);
		_log.Info() << "starting service " << serviceId << " from " << process.Path;

		std::unique_ptr<void, decltype(std::free) *> stack(malloc(StackSize), &std::free);
		if (!stack)
			throw io::SystemException("failed to allocate stack");

		int flags = CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | SIGCHLD;
		/*add NS flags here*/
		//flags |= CLONE_NEWNS;

		int pid = clone(&RunProcessTrampoline, static_cast<u8 *>(stack.get()) + StackSize, flags, &process);
		stack.reset();

		if (pid == -1)
			throw io::SystemException("clone");

		_log.Debug() << "spawned child with pid " << pid;

		while(!g_started)
		{
			_log.Debug() << "waiting for child to start...";
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		_log.Info() << "child process started";
	}

}}}
