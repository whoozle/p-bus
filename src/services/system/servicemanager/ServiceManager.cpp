#include <system/servicemanager/ServiceManager.h>

namespace pbus { namespace system { namespace servicemanager
{
	log::Logger Manager::_log("ServiceManagerImpl");

	void Manager::start(const std::string & name, u16 version)
	{
		ServiceId serviceId(name, version);
		_log.Info() << "starting service " << serviceId;
	}

}}}
