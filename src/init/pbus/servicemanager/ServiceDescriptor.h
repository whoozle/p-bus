#ifndef PBUS_SERVICEDESCRIPTOR_H
#define PBUS_SERVICEDESCRIPTOR_H

#include <toolkit/core/types.h>
#include <toolkit/log/Logger.h>
#include <string>
#include <vector>

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
		using ArgumentList = std::vector<std::string>;

		std::string					Executable;
		ArgumentList				Arguments;
		std::string					CurrentDirectory;
	};
}

#endif
