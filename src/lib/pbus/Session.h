#ifndef PBUS_SESSION_H
#define PBUS_SESSION_H

#include <pbus/idl/core/ICoreObject.h>
#include <pbus/LocalBusConnection.h>
#include <pbus/ComponentFactory.h>
#include <pbus/ClassId.h>
#include <pbus/MethodId.h>
#include <pbus/String.h>

#include <toolkit/log/Logger.h>
#include <toolkit/text/Formatters.h>
#include <toolkit/serialization/ISerializationStream.h>
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

		static constexpr u8 CommandInvoke			= 0;	//C → S
		static constexpr u8 CommandGet 				= 1;	//C → S
		static constexpr u8 CommandSet 				= 2;	//C → S
		static constexpr u8 CommandInvokeStatic		= 3;	//C → S
		static constexpr u8 Subscribe				= 4;	//C → S

		static constexpr u8 CommandResult 			= 0; 	//S → C
		static constexpr u8 CommandSignal 			= 1; 	//S → C
/*
	- method (object, method name, args) → [result | exception]
	- static method (method id, args) → [result | exception]
	- get property [object | typename] → [result | exception]
	- set property ([object | typename], value) → [result | exception]
	- signal( [object | typename], name)

*/

		mutable std::recursive_mutex							_lock;
		std::unordered_map<ClassId, IComponentFactoryPtr> 		_factories;
		std::unordered_map<ClassId, LocalBusConnectionPtr> 		_connections;
		std::unordered_map<ClassId, IServiceFactoryPtr> 		_services;

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

		void RegisterService(const ClassId &classId, IServiceFactoryPtr factory)
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			_services[classId] = factory;
		}

		IComponentFactoryPtr Get(const std::string &name) const
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto it = _factories.find(name);
			return it != _factories.end()? it->second: nullptr;
		}

		IServiceFactoryPtr GetService(const std::string &name) const
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto it = _services.find(name);
			return it != _services.end()? it->second: nullptr;
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
			static idl::core::ICoreObjectPtr object(factory->CreateRoot());
			auto result = std::dynamic_pointer_cast<InterfaceType>(object);
			if (!result)
				throw Exception("object created failed to cast");
			return result;
		}

		template<typename ReturnType, typename ... ArgumentType>
		std::promise<ReturnType> Invoke(const ObjectId & objectId, const MethodId & methodId, ArgumentType ... args)
		{
			_log.Debug() << "invoking " << objectId << "." << methodId.Name;
			std::promise<ReturnType> promise;
			auto connection = Connect(methodId.Service);
			ByteArray data;
			auto inserter = std::back_inserter(data.GetStorage());
			typename serialization::bson::OutputStream<decltype(inserter)> writer(inserter);
			objectId.Write(writer);
			writer.Write(methodId.Name);
			_log.Debug() << "data " << text::HexDump(data);

			promise.set_exception(std::make_exception_ptr(std::runtime_error("not implemented")));
			return promise;
		}

	private:
		LocalBusConnectionPtr Connect(const ClassId & id);
	};
}


#endif
