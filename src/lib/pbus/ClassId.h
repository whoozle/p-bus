#ifndef PBUS_CLASSID_H
#define PBUS_CLASSID_H

#include <toolkit/serialization/ISerializationStream.h>
#include <toolkit/text/StringOutputStream.h>
#include <toolkit/core/Exception.h>
#include <toolkit/core/Hash.h>
#include <string>
#include <stdlib.h>

namespace pbus
{
	struct ClassId
	{
		std::string		Name;
		uint			Version;

		// static auto GetClassDescriptor()
		// {
		// 	return
		// 		serialization::ClassDescriptor("ClassId", 1) &
		// 		serialization::Member(&ClassId::Name, "name") &
		// 		serialization::Member(&ClassId::Version, "version");
		// }

		ClassId(const std::string & name, uint version = 1)
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
			size_t operator()(const ClassId & id) const
			{ return CombineHash(_nameHash(id.Name), _versionHash(id.Version)); }
		};

		struct Equal
		{
			bool operator()(const ClassId & a, const ClassId & b) const
			{ return a == b; }
		};

		void ToString(text::StringOutputStream & ss) const
		{ ss << Name << '@' << Version; }

		bool operator == (const ClassId & o) const
		{ return Version == o.Version && Name == o.Name; }
		bool operator != (const ClassId & o) const
		{ return !((*this) == o); }

		void Write(serialization::ISerializationStream & stream) const;
		static ClassId Read(ConstBuffer buffer);

		TOOLKIT_DECLARE_SIMPLE_TOSTRING();
	};

	using ServiceId = ClassId;
}

TOOLKIT_DECLARE_STD_HASH(pbus::ClassId, pbus::ClassId::Hash)

#endif

