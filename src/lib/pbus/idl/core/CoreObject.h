#ifndef PBUS_IDL_COREOBJECT_H
#define PBUS_IDL_COREOBJECT_H

#include <pbus/idl/core/ICoreObject.h>
#include <pbus/Session.h>

namespace pbus { namespace idl { namespace core
{
	class CoreObject :
		public virtual ICoreObject,
		public virtual IRemoteObject
	{
	protected:
		ServiceId	_origin;
		ObjectId	_id;

	public:
		using InterfaceType = ICoreObject;

		CoreObject(const ServiceId & origin, const ObjectId & id):
			_origin(origin), _id(id)
		{ }

		const ObjectId & GetId() const override
		{ return _id; }

		const ServiceId & GetOrigin() const override
		{ return _origin; }
	};
}}}

#endif

