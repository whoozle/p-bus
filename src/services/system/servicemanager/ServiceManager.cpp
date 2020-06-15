#include <system/servicemanager/ServiceManager.h>
#include <toolkit/io/DirectoryReader.h>
#include <toolkit/io/File.h>
#include <toolkit/io/SystemException.h>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/mount.h>

namespace pbus { namespace system { namespace servicemanager
{
	namespace
	{
		enum struct EventType
		{
			Started,
			Stopped
		};

		struct Event
		{
			EventType 	Type;
			pid_t		Id;
		};

		io::FilePtr g_pipeRead, g_pipeWrite;

		void Write(Event event)
		{ g_pipeWrite->Write(ConstBuffer(reinterpret_cast<const u8 *>(&event), sizeof(event))); }

		void g_OnSigChld(int signo, siginfo_t * si, void * ctx)
		{ printf("SIGNAL CHLD\n"); Write(Event { EventType::Stopped, si->si_pid }); }

		void g_OnSigUSR2(int signo, siginfo_t * si, void * ctx)
		{ printf("SIGNAL USR3\n"); Write(Event { EventType::Started, si->si_pid }); }
	}
	log::Logger Manager::_log("ServiceManagerImpl");

	Manager::Manager()
	{
		std::string currentExe = io::File::ReadLink("/proc/self/exe");
		auto slashPos = currentExe.rfind('/');
		if (slashPos == currentExe.npos)
			throw Exception("could not find parent directory");

		auto slashPos2 = currentExe.rfind('/', slashPos - 1);
		if (slashPos == currentExe.npos)
			throw Exception("could not find root directory");

		{
			int readFd, writeFd;
			io::File::CreatePipe(readFd, writeFd);
			g_pipeRead = std::make_shared<io::File>(readFd);
			g_pipeWrite = std::make_shared<io::File>(writeFd);
			_poll.Add(*g_pipeRead, _event, io::Poll::EventInput);
		}

		_root = currentExe.substr(0, slashPos2);
		_log.Debug() << "root = " << _root;
		// _session = setsid();
		// if (_session == -1)
		// 	_log.Warning() << "setsid failed: " << io::SystemException::GetErrorMessage();

		struct sigaction act = {};
		act.sa_flags = SA_SIGINFO;
		act.sa_sigaction = g_OnSigChld;
		if (sigaction(SIGCHLD, &act, NULL) == -1)
			_log.Warning() << "failed to install SIGCHLD handler: " << io::SystemException::GetErrorMessage();

		act.sa_sigaction = &g_OnSigUSR2;
		act.sa_flags = SA_SIGINFO;
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
		//https://github.com/servo/servo/wiki/Linux-sandboxing
		// if (mount(NULL, "/", NULL, MS_PRIVATE|MS_REC, NULL) != 0)
		// 	perror("mount");
		// if (mount("./root", "/", "bind", MS_BIND|MS_RDONLY|MS_REC, NULL) != 0)
		// 	perror("remount");
		// if (chroot("./root") != 0)
		// 	perror("chroot");

		// io::DirectoryReader reader("/");
		// io::DirectoryReader::Entry entry;
		// while (reader.Read(entry))
		// {
		// 	_log.Info() << ">>" << entry.Name;
		// }

		if (execl(process.Path.c_str(), process.Path.c_str(), "--notify-parent", NULL) == -1) {
			_log.Error() << "exec failed: " << io::SystemException::GetErrorMessage();
		}
		exit(1);
		return 1;
	}

	std::string Manager::GetServicePath(const ServiceId & serviceId)
	{
		text::StringOutputStream os;
		os << _root << "/" << serviceId << "/" << serviceId;
		return os.Get();
	}

	void Manager::start(const std::string & name, u16 version)
	{
		ServiceId serviceId(name, version);

		Process process;
		process.Self = this;
		process.Path = GetServicePath(serviceId);
		_log.Info() << "starting service " << serviceId << " from " << process.Path;

		std::unique_ptr<void, decltype(std::free) *> stack(malloc(StackSize), &std::free);
		if (!stack)
			throw io::SystemException("failed to allocate stack");

		int flags = CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | SIGCHLD;
		/*add NS flags here*/
		flags |= CLONE_NEWNET | CLONE_NEWNS;

		auto pid = clone(&RunProcessTrampoline, static_cast<u8 *>(stack.get()) + StackSize, flags, &process);
		stack.reset();

		if (pid == -1)
			throw io::SystemException("clone");

		_log.Debug() << "spawned child with pid " << pid;

		bool started = false;
		while(!started)
		{
			_log.Debug() << "waiting for child to start...";
			//fixme: add timeout here
			if (_poll.Wait(10)) {
				Event event;
				if (g_pipeRead->Read(Buffer(reinterpret_cast<u8 *>(&event), sizeof(event))) != sizeof(event)) {
					throw Exception("short read");
				}
				switch(event.Type)
				{
				case EventType::Started:
					_log.Info() << "process " << event.Id << " started";
					if (event.Id != pid)
						throw Exception("spurious startup signal");
					started = true;
					break;
				case EventType::Stopped:
					_log.Warning() << "process " << event.Id << " stopped";
					if (pid == event.Id)
						throw Exception("process " + serviceId.ToString() + " failed to start");
					break;
				}
			}
		}
		_log.Info() << "child process started";
	}

}}}
