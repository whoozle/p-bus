#ifndef PBUS_SESSION_H
#define PBUS_SESSION_H

#include <toolkit/core/types.h>
#include <pbus/idl/ICoreObject.h>
#include <pbus/idl/IService.h>
#include <pbus/ServiceId.h>
#include <pbus/MethodId.h>
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
	class ComponentFactory : public IComponentFactory
	{
		ServiceId	_serviceId;
		size_t		_nextObjectId;

	public:
		ComponentFactory(ServiceId sessionId): _serviceId(sessionId), _nextObjectId(1)
		{ }

		idl::ICoreObject * Create(const SessionPtr & session) override
		{ return new Component(session, ObjectId(_serviceId, _nextObjectId++)); }
	};

	class LocalBusConnection;
	DECLARE_PTR(LocalBusConnection);

	class Session final : public std::enable_shared_from_this<Session>
	{
		static log::Logger										_log;

		std::recursive_mutex									_lock;
		std::unordered_map<ServiceId, IComponentFactoryPtr> 	_factories;
		std::unordered_map<ServiceId, LocalBusConnectionPtr> 	_connections;

	public:
		SessionPtr GetPointer()
		{ return shared_from_this();}

		template<typename Component>
		void Register(const ServiceId &serviceId)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto it = _factories.find(serviceId);
			if (it != _factories.end())
				return;
			_factories[serviceId] = std::make_shared<ComponentFactory<Component>>(serviceId);
		}

		template<typename Component>
		void Register(const ServiceId &serviceId, const IComponentFactoryPtr & factory)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto it = _factories.find(serviceId);
			if (it != _factories.end())
				return;
			_factories[serviceId] = factory;
		}

		IComponentFactoryPtr Get(const std::string &name)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto it = _factories.find(name);
			return it != _factories.end()? it->second: nullptr;
		}

		template<typename ProxyType>
		std::shared_ptr<typename ProxyType::InterfaceType> GetService(ServiceId serviceId)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto factory = Get(serviceId.ToString());
			if (!factory)
				throw Exception("no service " + serviceId.ToString() + " registered");

			auto connection = Session::Connect(serviceId);
			auto session = shared_from_this();
			idl::ICoreObjectPtr object(factory->Create(session));
			auto result = std::dynamic_pointer_cast<typename ProxyType::InterfaceType>(object);
			if (!result)
				throw Exception("object created failed to cast");
			return result;
		}

		template<typename ReturnType, typename ... ArgumentType>
		std::promise<ReturnType> Invoke(const MethodId & methodId, ArgumentType ... args)
		{
			std::promise<ReturnType> promise;
			auto connection = Connect(methodId.Service);
			promise.set_exception(std::make_exception_ptr(std::runtime_error("not implemented")));
			return promise;
		}

	private:
		LocalBusConnectionPtr Connect(const ServiceId & id);
	};
}


#endif
