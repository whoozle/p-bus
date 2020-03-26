#ifndef PBUS_OBJECTID_H
#define PBUS_OBJECTID_H

#include <toolkit/core/types.h>
#include <pbus/ClassId.h>
#include <string>

namespace pbus
{
	struct ObjectId
	{
		using IdType 		= u32;
		ClassId 	 		Service;
		IdType				Id;

		ObjectId(const ClassId & service, IdType id):
			Service(service), Id(id)
		{ }

		void ToString(text::StringOutputStream & ss) const
		{ ss << Service << "/" << Id; }

		class Hash
		{
			std::hash<decltype(Service)>		_service;
			std::hash<decltype(Id)>				_id;

		public:
			size_t operator()(const ObjectId & id) const
			{ return CombineHash(_service(id.Service), _id(id.Id)); }
		};

		struct Equal
		{
			bool operator()(const ObjectId & a, const ObjectId & b) const
			{ return a.Service == b.Service && a.Id == b.Id; }
		};

		bool operator == (const ObjectId & o) const
		{ return Service == o.Service && Id == o.Id; }
		bool operator != (const ObjectId & o) const
		{ return !((*this) == o); }

		TOOLKIT_DECLARE_SIMPLE_TOSTRING();
	};
}

TOOLKIT_DECLARE_STD_HASH(pbus::ObjectId, pbus::ObjectId::Hash)

#endif
