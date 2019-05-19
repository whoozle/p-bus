#ifndef PBUS_SESSION_H
#define PBUS_SESSION_H

#include <toolkit/core/types.h>
#include <pbus/idl/ICoreObject.h>
#include <pbus/idl/IService.h>
#include <pbus/ServiceId.h>
#include <pbus/String.h>
#include <pbus/LocalBusConnection.h>
#include <toolkit/log/Logger.h>
#include <memory>
#include <unordered_map>
#include <future>
#include <mutex>

namespace pbus
{
	class Session;
	DECLARE_PTR(Session);

	struct IComponentFactory
	{
		virtual ~IComponentFactory() = default;
		virtual idl::ICoreObject * Create(const SessionPtr & session) = 0;
	};
	DECLARE_PTR(IComponentFactory);

	template<typename Component>
	struct ComponentFactory : public IComponentFactory
	{
		idl::ICoreObject * Create(const SessionPtr & session) override
		{ return new Component(session); }
	};

	class LocalBusConnection;
	DECLARE_PTR(LocalBusConnection);

	class Session final : public std::enable_shared_from_this<Session>
	{
		static log::Logger										_log;

		std::recursive_mutex									_lock;
		std::unordered_map<String, IComponentFactoryPtr> 		_factories;
		std::unordered_map<ServiceId, LocalBusConnectionPtr> 	_connections;

	public:
		SessionPtr GetPointer()
		{ return shared_from_this();}

		template<typename Component>
		void Register(const std::string &name)
		{
			auto it = _factories.find(name);
			if (it != _factories.end())
				return;
			_factories[name] = std::make_shared<ComponentFactory<Component>>();
		}

		template<typename ProxyType>
		std::shared_ptr<typename ProxyType::InterfaceType> GetService(ServiceId serviceId)
		{
			using InterfaceType = typename ProxyType::InterfaceType;

			std::lock_guard<decltype(_lock)> l(_lock);
			auto connection = Session::Connect(serviceId);
			return nullptr;
		}

		template<typename ReturnType, typename ... ArgumentType>
		std::future<ReturnType> Invoke(ArgumentType ... args)
		{ }

	private:
		LocalBusConnectionPtr Connect(const ServiceId & id);
	};
}


#endif
