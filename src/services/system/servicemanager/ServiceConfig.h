#ifndef SERVICES_SYSTEM_SERVICEMANAGER_SERVICE_CONFIG_H
#define SERVICES_SYSTEM_SERVICEMANAGER_SERVICE_CONFIG_H

#include <set>
#include <string>

namespace pbus { namespace system { namespace servicemanager
{

	class Config
	{
	public:
		std::set<std::string> 	Dependencies;

		Config(const std::string &path);
	};

}}}

#endif
