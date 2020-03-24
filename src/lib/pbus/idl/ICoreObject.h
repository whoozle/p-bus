#ifndef PBUS_IDL_ICOREREMOTEOBJECT_H
#define PBUS_IDL_ICOREREMOTEOBJECT_H

#include <pbus/ObjectId.h>
#include <memory>

namespace pbus { namespace idl
{

	struct ICoreObject
	{
		static pbus::ClassId ClassId;

		virtual ~ICoreObject() = default;
	};
	TOOLKIT_DECLARE_PTR(ICoreObject);

	struct IRemoteObject
	{
		virtual ~IRemoteObject() = default;
		virtual const ObjectId & GetId() const = 0;
	};
	TOOLKIT_DECLARE_PTR(IRemoteObject);

}}

#endif

