#ifndef PBUS_SESSION_H
#define PBUS_SESSION_H

#include <pbus/idl/core/ICoreObject.h>
#include <pbus/LocalBusConnection.h>
#include <pbus/ClassId.h>
#include <pbus/MethodId.h>
#include <pbus/String.h>

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
		virtual idl::core::ICoreObject * Create() = 0;
	};
	TOOLKIT_DECLARE_PTR(IComponentFactory);

	template <typename InterfaceType>
	struct ITypedComponentFactory : public IComponentFactory
	{
		virtual InterfaceType * Create() = 0;
	};

	template <typename Component, typename InterfaceType = typename Component::InterfaceType>
	class ComponentFactory : public ITypedComponentFactory<InterfaceType>
	{
		ClassId		_serviceId;
		size_t		_nextObjectId;

	public:
		ComponentFactory(ClassId sessionId): _serviceId(sessionId), _nextObjectId(1)
		{ }

		InterfaceType * Create() override
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
		std::unordered_map<ClassId, IComponentFactoryPtr> 		_factories;
		std::unordered_map<ClassId, LocalBusConnectionPtr> 		_connections;

		Session();

	public:
		static Session & Get()
		{ static Session session; return session; }

		template<typename Component>
		void RegisterProxy(const ClassId &classId, bool force = false)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			if (!force)
			{
				auto it = _factories.find(classId);
				if (it != _factories.end())
					return;
			}
			_factories[classId] = std::make_shared<ComponentFactory<Component>>(classId);
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
			InterfaceType::RegisterProxy(*this);
			auto & classId = InterfaceType::ClassId;
			auto factory = Get(classId.ToString());
			if (!factory)
				throw Exception("no service " + classId.ToString() + " registered");

			auto connection = Session::Connect(classId);
			idl::core::ICoreObjectPtr object(factory->Create());
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
