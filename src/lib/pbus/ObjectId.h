#ifndef PBUS_OBJECTID_H
#define PBUS_OBJECTID_H

#include <toolkit/core/types.h>
#include <toolkit/serialization/ISerializationStream.h>
#include <pbus/ClassId.h>
#include <string>

namespace pbus
{
	struct ObjectId
	{
		using IdType 		= u32;
		ClassId 	 		Type;
		IdType				Id;

		ObjectId(const ClassId & type, IdType id):
			Type(type), Id(id)
		{ }

		void ToString(text::StringOutputStream & ss) const
		{ ss << Type << "/" << Id; }

		class Hash
		{
			std::hash<decltype(Type)>		_type;
			std::hash<decltype(Id)>			_id;

		public:
			size_t operator()(const ObjectId & id) const
			{ return CombineHash(_type(id.Type), _id(id.Id)); }
		};

		struct Equal
		{
			bool operator()(const ObjectId & a, const ObjectId & b) const
			{ return a.Type == b.Type && a.Id == b.Id; }
		};

		bool operator == (const ObjectId & o) const
		{ return Type == o.Type && Id == o.Id; }
		bool operator != (const ObjectId & o) const
		{ return !((*this) == o); }

		void Write(serialization::ISerializationStream & stream) const;

		TOOLKIT_DECLARE_SIMPLE_TOSTRING();
	};
}

TOOLKIT_DECLARE_STD_HASH(pbus::ObjectId, pbus::ObjectId::Hash)

#endif
