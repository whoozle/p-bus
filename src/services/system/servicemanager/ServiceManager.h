#ifndef PBUS_SERVICE_SYSTEM_SERVICE_MANAGER_H
#define PBUS_SERVICE_SYSTEM_SERVICE_MANAGER_H

#include <pbus/idl/system/IServiceManager.h>
#include <pbus/ClassId.h>
#include <toolkit/log/Logger.h>
#include <sys/types.h>


namespace pbus { namespace system { namespace servicemanager
{
	class Manager : public idl::system::IServiceManager
	{
		static log::Logger _log;

		std::string		_root;
		pid_t			_session;

	public:
		Manager();
		void start(const std::string & name, u16 version);
		std::string GetServicePath(const ServiceId & serviceId);
	};

}}}

#endif
