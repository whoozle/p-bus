#include <pbus/Session.h>
#include <pbus/idl/system/IServiceManager.h>
#include <pbus/LocalBusConnection.h>
#include <toolkit/text/Formatters.h>
#include <toolkit/serialization/bson/InputStream.h>

namespace pbus
{
	log::Logger Session::_log("session");

	Session::Session()
	{
		idl::system::IServiceManager::RegisterProxy(*this);
	}

	Session::~Session()
	{ }

	LocalBusConnectionPtr Session::Connect(const ServiceId & serviceId)
	{
		_log.Debug() << "connecting to " << serviceId;
		auto i = _connections.find(serviceId);
		if (i != _connections.end())
			return i->second;

		try
		{
			auto connection = std::make_shared<LocalBusConnection>(serviceId);
			_connections.insert(std::make_pair(serviceId, connection));
			return connection;
		}
		catch(const std::exception & ex)
		{
			_log.Debug() << "exception while connection: " << ex;
			if (serviceId == idl::system::IServiceManager::ClassId)
				throw Exception("cannot connect to ServiceManager to create service " + serviceId.ToString());

			_log.Debug() << "connecting to service manager at " << idl::system::IServiceManager::ClassId;
			auto serviceManager = GetService<idl::system::IServiceManager>();
			/*auto lease =*/ serviceManager->start(serviceId.Name, serviceId.Version);
			auto connection = std::make_shared<LocalBusConnection>(serviceId);
			_connections.insert(std::make_pair(serviceId, connection));
			return connection;
		}
	}

	void Session::ThrowException(const ServiceId & origin, u32 serial, const std::exception & ex)
	{
		_log.Debug() << "throwing exception " << ex << " to " << origin;
		try
		{ MakeRequest(origin, ResponseException, serial, "Exception", ex.what()); }
		catch(const std::exception & ex2)
		{ _log.Error() << "error while throwing exception of " << ex << ": " << ex2; }
	}

	u32 Session::Send(ServiceId service, ByteArray && data)
	{
		_log.Debug() << "sending to " << service << text::HexDump(data);
		auto connection = Connect(service);
		if (connection)
			return connection->Send(std::move(data));
		else
			throw Exception("no connection to service " + service.ToString() + " possible");
	}

	void Session::OnIncomingData(const ServiceId & serviceId, u32 serial, ConstBuffer data)
	{
		_log.Debug() << "incoming data from " << serviceId << ", serial " << serial << text::HexDump(data);
		try
		{
			size_t offset = 0;
			auto request = serialization::bson::ReadSingleValue<u8>(data, offset);
			_log.Debug() << "request type: " << request;
			switch(request)
			{
				case RequestInvoke:
					OnIncomingInvoke(serviceId, serial, ConstBuffer(data, offset));
					break;
				case ResponseException:
					OnIncomingException(serviceId, serial, ConstBuffer(data, offset));
					break;
				case ResponseResult:
					OnIncomingException(serviceId, serial, ConstBuffer(data, offset));
					break;
				default:
					throw Exception("unhandled request " + std::to_string(request));
			}
		}
		catch(const std::exception & ex)
		{ ThrowException(serviceId, serial, ex); }
	}

	void Session::OnIncomingInvoke(const ServiceId & origin, u32 serial, ConstBuffer data)
	{
		size_t offset = 0;
		std::string classType = serialization::bson::ReadSingleValue<std::string>(data, offset);
		s64 classVersion = serialization::bson::ReadSingleValue<u32>(data, offset);
		s64 objectId = serialization::bson::ReadSingleValue<u32>(data, offset);
		ObjectId id(ClassId(classType, classVersion), objectId);
		std::string methodName = serialization::bson::ReadSingleValue<std::string>(data, offset);
		_log.Info() << "invoke " << id << ", method: " << methodName << " args at " << text::Hex(offset, 4);


		idl::core::ICoreObjectPtr object;
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto it = _localObjects.find(id);
			if (it == _localObjects.end())
				throw Exception("object with id " + id.ToString() + " not found");

			object = it->second;
		}
	}

	IResponseParserPtr Session::PopResponseParser(const ServiceId & origin, u32 serial)
	{
		std::lock_guard<decltype(_lock)> l(_lock);
		auto oi = _requests.find(origin);
		if (oi == _requests.end())
			return nullptr;

		auto & requests = oi->second;
		auto i = requests.find(serial);
		if (i == requests.end())
			return nullptr;

		auto r = i->second;
		requests.erase(i);
		return r;
	}


	void Session::OnIncomingException(const ServiceId & origin, u32 serial, ConstBuffer data)
	{
		IResponseParserPtr response;
		try
		{
			size_t offset = 0;
			u32 responseSerial = serialization::bson::ReadSingleValue<u32>(data, offset);
			response = PopResponseParser(origin, responseSerial);
			if (!response)
				throw Exception("no response with serial " + std::to_string(responseSerial));

			std::string exceptionType = serialization::bson::ReadSingleValue<std::string>(data, offset);
			std::string exceptionArg1 = serialization::bson::ReadSingleValue<std::string>(data, offset);
			_log.Debug() << "ignoring exception type " << exceptionType << " for now";
			response->SetException(std::make_exception_ptr(std::runtime_error(exceptionArg1)));
		}
		catch(const std::exception & ex)
		{
			_log.Error() << "exception while parsing exception " << ex;
			if (response)
				response->SetException(std::current_exception());
		}
	}

	void Session::OnIncomingResult(const ServiceId & origin, u32 serial, ConstBuffer data)
	{
		_log.Error() << "implement parser for response:" << text::HexDump(data);
	}

}
