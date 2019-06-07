#ifndef PBUS_METHODID_H
#define PBUS_METHODID_H

#include <toolkit/core/types.h>
#include <toolkit/text/StringOutputStream.h>
#include <string>

namespace pbus
{
	struct MethodId
	{
		ServiceId & 	Service;
		std::string		Name;

		MethodId(ServiceId & service, const std::string & name):
			Service(service), Name(name)
		{ }

		void ToString(text::StringOutputStream & ss) const
		{ ss << Service << "::" << Name; }

		TOOLKIT_DECLARE_SIMPLE_TOSTRING();
	};
}

#endif
