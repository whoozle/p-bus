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
		ObjectId	_id;

	public:
		using InterfaceType = ICoreObject;

		CoreObject(const ObjectId & id): _id(id)
		{ }

		const ObjectId & GetId() const override
		{ return _id; }
	};
}}}

#endif

