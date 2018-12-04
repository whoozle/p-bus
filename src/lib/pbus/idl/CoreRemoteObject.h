#ifndef PBUS_IDL_COREOBJECT_H
#define PBUS_IDL_COREOBJECT_H

#include <pbus/idl/ICoreRemoteObject.h>

namespace pbus { namespace idl
{
	class CoreRemoteObject
	{
		SessionPtr _session;

	public:
		CoreRemoteObject(const SessionPtr & session): _session(session) { }
	};
}}

#endif

