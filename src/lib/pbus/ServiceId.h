#ifndef PBUS_SERVICEID_H
#define PBUS_SERVICEID_H

#include <toolkit/text/StringOutputStream.h>
#include <toolkit/core/Exception.h>
#include <toolkit/core/Hash.h>
#include <toolkit/core/types.h>
#include <string>
#include <stdlib.h>

namespace pbus
{
	struct ServiceId
	{
		std::string		Name;
		uint			Version;

		ServiceId(const std::string & name, uint version = 1)
		{
			auto pos = name.find('@');
			if (pos != name.npos)
			{
				Name = name.substr(0, pos);
				Version = atoi(name.substr(pos + 1).c_str());
				if (Version <= 0)
					throw Exception("invalid version");
			}
			else
			{
				Name = name;
				Version = 1;
			}
		}

		class Hash
		{
			std::hash<decltype(Name)>		_nameHash;
			std::hash<decltype(Version)>	_versionHash;

		public:
			size_t operator()(const ServiceId & id) const
			{ return CombineHash(_nameHash(id.Name), _versionHash(id.Version)); }
		};

		struct Equal
		{
			bool operator()(const ServiceId & a, const ServiceId & b) const
			{ return a.Name == b.Name && a.Version == b.Version; }
		};

		void ToString(text::StringOutputStream & ss) const
		{ ss << Name << '@' << Version; }

		TOOLKIT_DECLARE_SIMPLE_TOSTRING();
	};
}

TOOLKIT_DECLARE_STD_HASH(pbus::ServiceId, pbus::ServiceId::Hash)

#endif

