#ifndef PBUS_SERVICE_SYSTEM_SERVICE_MANAGER_H
#define PBUS_SERVICE_SYSTEM_SERVICE_MANAGER_H

#include <pbus/idl/system/IServiceManager.h>
#include <pbus/ClassId.h>
#include <toolkit/io/IPollEventHandler.h>
#include <toolkit/io/Poll.h>
#include <toolkit/log/Logger.h>
#include <sys/types.h>


namespace pbus { namespace system { namespace servicemanager
{
	class Manager : public idl::system::IServiceManager
	{
		static log::Logger _log;
		static constexpr size_t StackSize = 1024 * 1024;

		std::string		_root;
		pid_t			_session;
		io::Poll		_poll;

		struct SocketEvent : public io::IPollEventHandler
		{
			int Event = 0;

			void HandleSocketEvent(int event) override
			{ Event = event; }
		};
		SocketEvent		_event;

		std::string GetServicePath(const ServiceId & serviceId);

		struct Process
		{
			Manager * 		Self;
			std::string 	Path;
		};

		static int RunProcessTrampoline(void * arg);
		int RunProcess(const Process & process);

	public:
		Manager();

	//idl:
		void start(const std::string & name, u16 version);
	};

}}}

#endif
