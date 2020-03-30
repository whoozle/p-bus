#ifndef PBUS_APPLICATION_H
#define PBUS_APPLICATION_H

#include <pbus/ClassId.h>
#include <pbus/Session.h>

namespace pbus
{
	class LocalBus;
	TOOLKIT_DECLARE_PTR(LocalBus);

	class Application
	{
		log::Logger 	_log;
		LocalBusPtr  	_bus;
		bool			_notifyParent;

	private:
		template<typename ClassType>
		struct ServiceComponentFactory : public IServiceFactory
		{
			idl::core::ICoreObjectPtr Get() override
			{
				static idl::core::ICoreObjectPtr instance(new ClassType);
				return instance;
			}
		};

	public:
		Application(const ServiceId & serviceId, int argc, char ** argv);
		~Application();

		template<typename Service>
		void RegisterService()
		{
			auto & session = Session::Get();
			session.RegisterService(Service::ClassId, std::make_shared<ServiceComponentFactory<Service>>());
		}

		void Run();
	};
}

#endif
