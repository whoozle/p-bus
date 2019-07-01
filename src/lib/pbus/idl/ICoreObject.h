#ifndef PBUS_IDL_ICOREREMOTEOBJECT_H
#define PBUS_IDL_ICOREREMOTEOBJECT_H

#include <pbus/ObjectId.h>

namespace pbus { namespace idl
{

	struct ICoreObject
	{
		virtual ~ICoreObject() = default;
	};
	DECLARE_PTR(ICoreObject);

	struct IRemoteObject
	{
		virtual ~IRemoteObject() = default;
		virtual const ObjectId & GetId() const = 0;
	};
	DECLARE_PTR(IRemoteObject);

}}

#endif

