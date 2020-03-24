#ifndef PBUS_METHODID_H
#define PBUS_METHODID_H

#include <toolkit/core/types.h>
#include <toolkit/text/StringOutputStream.h>
#include <toolkit/serialization/ISerializationStream.h>
#include <toolkit/serialization/Serialization.h>
#include <string>

namespace pbus
{
	struct MethodId
	{
		const ClassId & 	Service;
		std::string			Name;

		MethodId(const ClassId & service, const std::string & name):
			Service(service), Name(name)
		{ }

		void ToString(text::StringOutputStream & ss) const
		{ ss << Service << "::" << Name; }

		// static auto GetClassDescriptor()
		// {
		// 	return
		// 		serialization::ClassDescriptor("MethodId", 1) &
		// 		serialization::Member(&MethodId::Service, "method") &
		// 		serialization::Member(&MethodId::Name, "name");
		// }

		TOOLKIT_DECLARE_SIMPLE_TOSTRING();
	};
}

#endif
