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
			auto lease = serviceManager->start(serviceId.Name, serviceId.Version);
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
			connection->Send(std::move(data));
		else
			throw Exception("no connection to service " + service.ToString() + " possible");
	}

	void Session::OnIncomingData(const ServiceId & serviceId, ConstBuffer data)
	{
		_log.Debug() << "incoming data from " << serviceId << text::HexDump(data);
		serialization::bson::SingleValueParser<s64> request;
		size_t offset = 0;
		request.Parse(data, offset);
		if (!request.Finished())
			throw Exception("could not parse request");
		_log.Info() << "request: " << request.Value;
	}

}
