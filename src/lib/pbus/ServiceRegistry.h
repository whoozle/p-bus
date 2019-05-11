#ifndef PBUS_SERVICEREGISTRY_H
#define PBUS_SERVICEREGISTRY_H

#include <toolkit/core/types.h>
#include <toolkit/log/Logger.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace pbus
{
	struct ServiceId
	{
		std::string		Name;
		uint			Version;

		ServiceId(const std::string & name, uint version = 1): Name(name), Version(version) { }

		class Hash
		{
			std::hash<decltype(Name)>		_nameHash;
			std::hash<decltype(Version)>	_versionHash;

		public:
			size_t operator()(const ServiceId & id) const
			{
				return _nameHash(id.Name) + _versionHash(id.Version);
			}
		};

		struct Equal
		{
			bool operator()(const ServiceId & a, const ServiceId & b) const
			{ return a.Name == b.Name && a.Version == b.Version; }
		};

		void ToString(text::StringOutputStream & ss) const
		{ ss << Name << '@' << Version; }
	};

	struct ServiceDescriptor
	{
		std::string					Executable;
		std::vector<std::string>	Arguments;
		std::string					CurrentDirectory;
	};

	class ServiceRegistry
	{
		using ServiceMap = std::unordered_map<ServiceId, ServiceDescriptor, ServiceId::Hash, ServiceId::Equal>;

		static log::Logger			_log;

		ServiceMap					_services;

	public:

		void Add(ServiceId id, ServiceDescriptor && desc);
	};
}

#endif
