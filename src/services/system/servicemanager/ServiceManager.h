#ifndef PBUS_SERVICE_SYSTEM_SERVICE_MANAGER_H
#define PBUS_SERVICE_SYSTEM_SERVICE_MANAGER_H

#include <pbus/idl/system/IServiceManager.h>
#include <toolkit/log/Logger.h>

namespace pbus { namespace system { namespace servicemanager
{
	class Manager : public idl::system::IServiceManager
	{
		static log::Logger _log;
	public:
		void start(const std::string & name, u16 version);
	};

}}}

#endif
