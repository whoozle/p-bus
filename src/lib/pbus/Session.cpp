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
			_log.Debug() << "exception while connection: " << ex.what();
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
		size_t offset = 0;
		auto request = serialization::bson::ReadSingleValue<u8>(data, offset);
		_log.Debug() << "request type: " << request;
		switch(request)
		{
			case RequestInvoke:
				OnIncomingInvoke(serviceId, serial, ConstBuffer(data, offset));
				break;
			default:
				throw Exception("unhandled request " + std::to_string(request));
		}
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

}
