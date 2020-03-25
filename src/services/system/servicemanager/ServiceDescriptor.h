#ifndef PBUS_SERVICEDESCRIPTOR_H
#define PBUS_SERVICEDESCRIPTOR_H

#include <toolkit/core/types.h>
#include <toolkit/log/Logger.h>
#include <string>
#include <vector>

namespace pbus
{
	struct ServiceDescriptor
	{
		using ArgumentList = std::vector<std::string>;

		std::string					Executable;
		ArgumentList				Arguments;
		std::string					CurrentDirectory;
	};
}

#endif
