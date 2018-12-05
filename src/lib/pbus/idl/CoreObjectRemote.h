#ifndef PBUS_IDL_COREOBJECT_H
#define PBUS_IDL_COREOBJECT_H

#include <pbus/idl/ICoreObject.h>
#include <pbus/Session.h>

namespace pbus { namespace idl
{
	class CoreObjectRemote
	{
		SessionPtr _session;

	public:
		CoreObjectRemote(const SessionPtr & session): _session(session) { }
	};
}}

#endif

