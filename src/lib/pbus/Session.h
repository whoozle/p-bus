#ifndef PBUS_SESSION_H
#define PBUS_SESSION_H

#include <toolkit/core/types.h>
#include <pbus/idl/ICoreObject.h>
#include <pbus/idl/IService.h>
#include <pbus/ClassId.h>
#include <pbus/MethodId.h>
#include <pbus/String.h>
#include <pbus/LocalBusConnection.h>

#include <toolkit/log/Logger.h>
#include <toolkit/serialization/Serializator.h>
#include <toolkit/serialization/ISerializationStream.h>
#include <toolkit/serialization/Serialization.h>
#include <toolkit/serialization/bson/OutputStream.h>

#include <future>
#include <memory>
#include <mutex>
#include <tuple>
#include <vector>
#include <unordered_map>

namespace pbus
{
	class Session;
	TOOLKIT_DECLARE_PTR(Session);

	struct IComponentFactory
	{
		virtual ~IComponentFactory() = default;
		virtual idl::ICoreObject * Create() = 0;
	};
	TOOLKIT_DECLARE_PTR(IComponentFactory);

	template<typename Component>
	class ComponentFactory : public IComponentFactory
	{
		ClassId		_serviceId;
		size_t		_nextObjectId;

	public:
		ComponentFactory(ClassId sessionId): _serviceId(sessionId), _nextObjectId(1)
		{ }

		idl::ICoreObject * Create() override
		{ return new Component(ObjectId(_serviceId, _nextObjectId++)); }
	};

	template<typename ... ArgumentType>
	class SessionRequest
	{
		MethodId & 						Method;
		std::tuple<ArgumentType & ...> 	Arguments;
	};

	class LocalBusConnection;
	TOOLKIT_DECLARE_PTR(LocalBusConnection);

	class Session final
	{
		static log::Logger										_log;

		std::recursive_mutex									_lock;
		std::unordered_map<ClassId, IComponentFactoryPtr> 	_factories;
		std::unordered_map<ClassId, LocalBusConnectionPtr> 	_connections;

		Session();

	public:
		static Session & Get()
		{ static Session session; return session; }

		template<typename Component>
		void Register(const ClassId &serviceId)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto it = _factories.find(serviceId);
			if (it != _factories.end())
				return;
			_factories[serviceId] = std::make_shared<ComponentFactory<Component>>(serviceId);
		}

		template<typename Component>
		void Register(const ClassId &serviceId, const IComponentFactoryPtr & factory)
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

		template<typename InterfaceType>
		std::shared_ptr<InterfaceType> GetService()
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto & classId = InterfaceType::ClassId;
			auto factory = Get(classId.ToString());
			if (!factory)
				throw Exception("no service " + classId.ToString() + " registered");

			auto connection = Session::Connect(classId);
			idl::ICoreObjectPtr object(factory->Create());
			auto result = std::dynamic_pointer_cast<InterfaceType>(object);
			if (!result)
				throw Exception("object created failed to cast");
			return result;
		}

		template<typename ReturnType, typename ... ArgumentType>
		std::promise<ReturnType> Invoke(const MethodId & methodId, ArgumentType ... args)
		{
			std::promise<ReturnType> promise;
			auto connection = Connect(methodId.Service);
			std::vector<u8> data;
			auto inserter = std::back_inserter(data);
			typename serialization::bson::OutputStream<decltype(inserter)> writer(inserter);

			promise.set_exception(std::make_exception_ptr(std::runtime_error("not implemented")));
			return promise;
		}

	private:
		LocalBusConnectionPtr Connect(const ClassId & id);
	};
}


#endif
