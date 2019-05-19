#ifndef PBUS_IDL_COREOBJECT_H
#define PBUS_IDL_COREOBJECT_H

#include <pbus/idl/ICoreObject.h>
#include <pbus/Session.h>

namespace pbus { namespace idl
{
	class CoreObject : public virtual ICoreObject
	{
	protected:
		SessionPtr _session;

	public:
		CoreObject(const SessionPtr & session): _session(session) { }
	};
}}

#endif

