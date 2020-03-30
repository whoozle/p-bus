#include <pbus/Session.h>
#include <pbus/LocalBusConnection.h>
#include <pbus/idl/system/ServiceManager.h>
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

	void Session::AddConnection(const ServiceId & serviceId, const LocalBusConnectionPtr & connection)
	{
		std::lock_guard<decltype(_lock)> l(_lock);
		_connections[serviceId] = connection;
	}

	idl::core::ICoreObjectPtr Session::GetLocalService(const ServiceId &serviceId)
	{
		std::lock_guard<decltype(_lock)> l(_lock);
		auto it = _services.find(serviceId);
		if (it == _services.end())
			return nullptr;

		_log.Debug() << "found local service, returning...";
		auto service = it->second->Get();
		ObjectId serviceObjectId(serviceId, 0);
		_localObjects.insert(std::make_pair(serviceObjectId, service));
		return service;
	}

	LocalBusConnectionPtr Session::Connect(const ServiceId & serviceId)
	{
		std::lock_guard<decltype(_lock)> l(_lock);
		auto i = _connections.find(serviceId);
		if (i != _connections.end())
			return i->second;

		_log.Debug() << "connecting to " << serviceId;
		auto connection = std::make_shared<LocalBusConnection>(serviceId);
		_connections.insert(std::make_pair(serviceId, connection));
		return connection;
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
		_log.Debug() << "invoke " << id << ", method: " << methodName << " args data " << text::HexDump(ConstBuffer(data, offset));

		idl::core::ICoreObjectPtr object;
		{
			std::lock_guard<decltype(_lock)> l(_lock);
			auto it = _localObjects.find(id);
			if (it == _localObjects.end() && id.Id == 0)
			{
				GetLocalService(id.Type); //just to register service instance
				it = _localObjects.find(id);
			}
			if (it == _localObjects.end())
				throw Exception("object with id " + id.ToString() + " not found");

			object = it->second;
		}

		ByteArray result;
		result.Reserve(4096);
		result.Resize(HeaderSize);
		auto inserter = std::back_inserter(result.GetStorage());
		typename serialization::bson::OutputStream<decltype(inserter)> writer(inserter);
		serialization::Serialize(writer, (u8)ResponseResult, serial);
		object->__pbus__invoke(writer, methodName, ConstBuffer(data, offset));
		io::LittleEndianDataOutputStream::WriteU32(result.data(), result.size() - HeaderSize);
		_log.Debug() << "method result" << text::HexDump(result);
		Send(origin, std::move(result));
	}

	IResponseParserPtr Session::GetResponseParser(const ServiceId & origin, u32 serial)
	{
		std::lock_guard<decltype(_lock)> l(_lock);
		auto oi = _requests.find(origin);
		if (oi == _requests.end())
			return nullptr;

		auto & requests = oi->second;
		auto i = requests.find(serial);
		return i != requests.end()? i->second: nullptr;
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
		if (r->Finished())
			requests.erase(i);
		return r;
	}

	IResponseParserPtr Session::WaitResponse(const ServiceId & origin, u32 serial)
	{
		_log.Debug() << "waiting...";
		while (_poll.Wait(15000) != 0)
		{
			auto response = GetResponseParser(origin, serial);
			_log.Debug() << "registered response " << response.get();
			if (response && response->Finished()) {
				_log.Debug() << "got response for " << origin << " #" << serial;
				return response;
			}
			_log.Debug() << "waiting again...";
		}
		throw Exception("Timed out waiting for reply");
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
			_log.Debug() << "setting exception text to \"" << exceptionArg1 << "\" for request " << origin << " #" << responseSerial;
			_log.Debug() << " response ptr " << response.get();
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
