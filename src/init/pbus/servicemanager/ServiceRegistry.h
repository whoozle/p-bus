#ifndef PBUS_SERVICEREGISTRY_H
#define PBUS_SERVICEREGISTRY_H

#include <pbus/servicemanager/ServiceDescriptor.h>
#include <pbus/ClassId.h>
#include <toolkit/log/Logger.h>
#include <toolkit/core/types.h>
#include <string>
#include <unordered_map>

namespace pbus
{
	class ServiceRegistry
	{
		using ServiceMap = std::unordered_map<ClassId, ServiceDescriptor, ClassId::Hash, ClassId::Equal>;

		static log::Logger			_log;

		ServiceMap					_services;

	public:
		void Add(const ClassId & id, ServiceDescriptor && desc);
		const ServiceDescriptor * GetDescriptor(const ClassId & id) const;
	};
}

#endif
