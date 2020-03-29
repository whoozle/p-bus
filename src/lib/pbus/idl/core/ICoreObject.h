#ifndef PBUS_IDL_ICOREREMOTEOBJECT_H
#define PBUS_IDL_ICOREREMOTEOBJECT_H

#include <pbus/ObjectId.h>
#include <memory>
namespace pbus { namespace serialization { struct ISerializationStream; }}

namespace pbus { namespace idl { namespace core
{

	struct ICoreObject
	{
		static pbus::ClassId ClassId;

		virtual ~ICoreObject() = default;
		virtual void __pbus__invoke(serialization::ISerializationStream & resultStream, const std::string & method, ConstBuffer argsData);
	};
	TOOLKIT_DECLARE_PTR(ICoreObject);

	struct IRemoteObject
	{
		virtual ~IRemoteObject() = default;
		virtual const ObjectId & GetId() const = 0;
		virtual const ServiceId & GetOrigin() const = 0;
	};
	TOOLKIT_DECLARE_PTR(IRemoteObject);

}}}

#endif

