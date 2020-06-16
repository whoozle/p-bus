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

	void Manager::Bind(const std::string src, const std::string dst)
	{
		_log.Info() << "bind " << src << " " << dst;
		if (mount(src.c_str(), dst.c_str(), "bind", MS_BIND|MS_RDONLY|MS_REC, NULL) != 0)
			throw io::SystemException("mount " + src + " " + dst);
	}

	int Manager::RunProcess(const Process & process)
	{
		//https://github.com/servo/servo/wiki/Linux-sandboxing
		auto root = GetServiceRoot(process.Id);
		_log.Info() << "root = " << root;
		if (mount(NULL, "/", NULL, MS_PRIVATE|MS_REC, NULL) != 0)
			throw io::SystemException("mount / [private]");
		if (mount("tmpfs", root.c_str(), "tmpfs", 0, "size=1m") != 0)
			throw io::SystemException("mount tmpfs");

		auto libDir = root + "/lib";
		auto packagesDir = root + "/packages";

		if (mkdir(libDir.c_str(), 0700) != 0)
			throw io::SystemException("mkdir " + libDir);

		if (mkdir(packagesDir.c_str(), 0700) != 0)
			throw io::SystemException("mkdir " + libDir);

		Bind(GetSourcePath("lib"), libDir);

		for (auto dep : process.Dependencies) {
			auto serviceDir = packagesDir + "/" + dep.ToString();
			if (mkdir(serviceDir.c_str(), 0700) != 0)
				throw io::SystemException("mkdir " + serviceDir);
			Bind(GetSourcePath("packages/" + dep.ToString()), serviceDir);
		}

		if (chroot(root.c_str()) != 0)
			throw io::SystemException("chroot");
		if (chdir("/") != 0)
			throw io::SystemException("chdir /");

		io::DirectoryReader reader("/packages/system.RandomGenerator-1");
		io::DirectoryReader::Entry entry;
		while (reader.Read(entry))
		{
			_log.Info() << ">>" << entry.Name;
		}

		if (execl(process.Path.c_str(), process.Path.c_str(), "--notify-parent", NULL) == -1) {
			_log.Error() << "exec " << process.Path << " failed: " << io::SystemException::GetErrorMessage();
		}
		exit(1);
		return 1;
	}

	std::string Manager::GetServiceRoot(const ServiceId & serviceId)
	{
		text::StringOutputStream os;
		os << _root << "/" << serviceId << "/root";
		return os.Get();
	}

	std::string Manager::GetSourcePath(const std::string &path)
	{
#ifdef PBUS_DEVEL_MODE
		return "./" + path;
#else
		return "/" + path;
#endif
	}

	std::string Manager::GetServicePath(const ServiceId & serviceId)
	{
		text::StringOutputStream os;
		os << "/packages/" << serviceId << "/" << serviceId;
		return os.Get();
	}

	void Manager::start(const std::string & name, u16 version)
	{
		ServiceId serviceId(name, version);

		Process process(this, serviceId, GetServicePath(serviceId));
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
