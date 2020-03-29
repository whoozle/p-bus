#ifndef PBUS_SESSION_H
#define PBUS_SESSION_H

#include <pbus/idl/core/ICoreObject.h>
#include <pbus/ComponentFactory.h>
#include <pbus/ClassId.h>
#include <pbus/MethodId.h>
#include <pbus/ResponseParser.h>
#include <pbus/String.h>

#include <toolkit/log/Logger.h>
#include <toolkit/io/DataStream.h>
#include <toolkit/io/Poll.h>
#include <toolkit/serialization/ISerializationStream.h>
#include <toolkit/serialization/Serializator.h>
#include <toolkit/serialization/bson/OutputStream.h>

#include <memory>
#include <mutex>
#include <tuple>
#include <vector>
#include <unordered_map>

namespace pbus
{
	class Session;
	TOOLKIT_DECLARE_PTR(Session);

	class LocalBusConnection;
	TOOLKIT_DECLARE_PTR(LocalBusConnection);

	class Session final
	{
		static log::Logger										_log;

		static constexpr u8 RequestInvoke			= 0;	//C → S
		static constexpr u8 RequestGet 				= 1;	//C → S
		static constexpr u8 RequestSet 				= 2;	//C → S
		static constexpr u8 RequestInvokeStatic		= 3;	//C → S
		static constexpr u8 RequestSubscribe		= 4;	//C → S
		static constexpr u8 RequestRelease			= 5;	//C → S

		static constexpr u8 ResponseResult 			= 6; 	//S → C
		static constexpr u8 ResponseException		= 7; 	//S → C
		static constexpr u8 ResponseSignal 			= 8; 	//S → C
/*
	- method (object, method name, args) → [result | exception]
	- static method (method id, args) → [result | exception]
	- get property [object | typename] → [result | exception]
	- set property ([object | typename], value) → [result | exception]
	- signal( [object | typename], name)
*/

		mutable std::recursive_mutex							_lock;
		io::Poll												_poll;
		std::unordered_map<ClassId, IComponentFactoryPtr> 		_factories;
		std::unordered_map<ClassId, LocalBusConnectionPtr> 		_connections;
		std::unordered_map<ServiceId, IServiceFactoryPtr> 		_services;
		std::unordered_map<ServiceId, std::unordered_map<u32, IResponseParserPtr>>	_requests;
		std::unordered_map<ObjectId, idl::core::ICoreObjectPtr>	_localObjects;

		Session();
		~Session();

	public:
		static Session & Get()
		{ static Session session; return session; }

		io::Poll & GetPoll()
		{ return _poll; }

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
			_log.Debug() << "registering factory for proxy class " << classId;
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

		void AddConnection(const ServiceId & service, const LocalBusConnectionPtr & connection);

		template<typename InterfaceType>
		std::shared_ptr<InterfaceType> GetService()
		{
			auto & classId = InterfaceType::ClassId;
			_log.Debug() << "GetService " << classId;

			std::lock_guard<decltype(_lock)> l(_lock);
			auto it = _services.find(classId);
			if (it != _services.end())
			{
				_log.Debug() << "found local service, returning...";
				auto service = std::dynamic_pointer_cast<InterfaceType>(it->second->Get());
				if (!service)
					throw Exception("service registered can't cast to interface for " + classId.ToString());
				return service;
			}

			InterfaceType::RegisterProxy(*this);
			_log.Debug() << "GetService " << classId;
			auto factory = Get(classId.ToString());
			if (!factory)
				throw Exception("no service " + classId.ToString() + " registered");

			static idl::core::ICoreObjectPtr object(InterfaceType::CreateProxy(classId, ObjectId(classId, 0)));
			auto result = std::dynamic_pointer_cast<InterfaceType>(object);
			if (!result)
				throw Exception("object created failed to cast");
			return result;
		}

		template <typename ... ArgumentType>
		u32 MakeRequest(const ServiceId & origin, u8 RequestType, ArgumentType ... args)
		{
			static constexpr size_t HeaderSize = 4;
			ByteArray data;
			data.Reserve(4096);
			data.Resize(HeaderSize);
			auto inserter = std::back_inserter(data.GetStorage());
			typename serialization::bson::OutputStream<decltype(inserter)> writer(inserter);
			serialization::Serialize(writer, RequestType, args...);
			io::LittleEndianDataOutputStream::WriteU32(data.data(), data.size() - HeaderSize);
			return Send(origin, std::move(data));
		}

		template<typename ReturnType, typename ... ArgumentType>
		u32 Invoke(const ServiceId & origin, const ObjectId & objectId, const MethodId & methodId, ArgumentType ... args)
		{
			_log.Debug() << "invoking " << objectId << "." << methodId.Name;
			u32 serial = MakeRequest(origin, RequestInvoke, objectId, methodId.Name, args...);
			auto parser = std::make_shared<ResponseParser<ReturnType>>();
			_requests[origin][serial] = parser;
			return serial;
		}

		template<typename ReturnType>
		ReturnType Wait(const ServiceId & origin, u32 serial)
		{
			_log.Debug() << "Wait " << serial;
			auto response = WaitResponse(origin, serial);
			auto exception = response->GetException();
			if (exception)
				std::rethrow_exception(exception);

			using ParserType = ResponseParser<ReturnType>;
			auto result = std::dynamic_pointer_cast<ParserType>(response);
			if (!result)
				throw Exception("mismatched type for response parser");

			return result->GetValue();
		}

		void Release(const ServiceId & origin, const ObjectId & objectId)
		{
			_log.Debug() << "releasing " << objectId;
			try
			{ MakeRequest(origin, RequestRelease, objectId); }
			catch(const std::exception & ex)
			{ _log.Error() << "releasing " << objectId << " failed: " << ex; }
		}

		void OnIncomingData(const ServiceId & origin, u32 serial, ConstBuffer data);

	private:
		LocalBusConnectionPtr Connect(const ServiceId & id);

		u32 Send(ServiceId, ByteArray && data);

		void OnIncomingInvoke(const ServiceId & origin, u32 serial, ConstBuffer data);

		IResponseParserPtr WaitResponse(const ServiceId & origin, u32 serial);
		IResponseParserPtr GetResponseParser(const ServiceId & origin, u32 serial);
		IResponseParserPtr PopResponseParser(const ServiceId & origin, u32 serial);
		void OnIncomingException(const ServiceId & origin, u32 serial, ConstBuffer data);
		void OnIncomingResult(const ServiceId & origin, u32 serial, ConstBuffer data);

		void ThrowException(const ServiceId & origin, u32 serial, const std::exception & ex);
	};
}


#endif
