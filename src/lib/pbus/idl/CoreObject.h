#ifndef PBUS_IDL_COREOBJECT_H
#define PBUS_IDL_COREOBJECT_H

#include <pbus/idl/ICoreObject.h>
#include <pbus/Session.h>

namespace pbus { namespace idl
{
	class CoreObject :
		public virtual ICoreObject,
		public virtual IRemoteObject
	{
	protected:
		SessionPtr 	_session;
		ObjectId	_id;

	public:
		CoreObject(const SessionPtr & session, const ObjectId & id): _session(session), _id(id)
		{ }
		const ObjectId & GetId() const override
		{ return _id; }
	};
}}

#endif

