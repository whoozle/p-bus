#include <system/servicemanager/ServiceManager.h>
#include <toolkit/io/SystemException.h>
#include <unistd.h>
#include <linux/limits.h>

namespace pbus { namespace system { namespace servicemanager
{
	log::Logger Manager::_log("ServiceManagerImpl");

	Manager::Manager()
	{
		char buf[PATH_MAX];
		auto size = readlink("/proc/self/exe", buf, sizeof(buf));
		if (size == -1)
			throw io::SystemException("readlink");

		std::string currentExe(buf, buf + size);
		auto slashPos = currentExe.rfind('/');
		if (slashPos == currentExe.npos)
			throw Exception("could not find root directory");
		_root = currentExe.substr(0, slashPos);
		_log.Debug() << "root = " << _root;
		_session = setsid();
		if (_session == -1)
			_log.Warning() << "setsid failed: " << io::SystemException::GetErrorMessage();
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
		auto exe = GetServicePath(serviceId);
		_log.Info() << "starting service " << serviceId << " from " << exe;
		int pid = vfork();
		if (pid == -1)
			throw io::SystemException("fork");

		if (pid == 0)
		{
			if (execl(exe.c_str(), exe.c_str(), NULL) == -1) {
				_log.Error() << "exec failed: " << io::SystemException::GetErrorMessage();
			}
			exit(1);
		}
		_log.Debug() << "spawned child with pid " << pid;
	}

}}}
