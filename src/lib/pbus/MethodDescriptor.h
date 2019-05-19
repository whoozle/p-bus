#ifndef PBUS_METHODDESCRIPTOR_H
#define PBUS_METHODDESCRIPTOR_H

#include <pbus/String.h>
#include <vector>

namespace pbus
{
	struct MethodDescriptor
	{
		String 					Name;
		String					ReturnType;
		std::vector<String> 	Types;

		MethodDescriptor(const String & rtype, const String &name, const std::initializer_list<String> &types):
			Name(name), ReturnType(rtype), Types(types)
		{ }
	};
}

#endif
