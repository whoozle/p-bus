#ifndef PBUS_SERVICEREGISTRY_H
#define PBUS_SERVICEREGISTRY_H

#include <pbus/servicemanager/ServiceDescriptor.h>
#include <toolkit/log/Logger.h>
#include <toolkit/core/types.h>
#include <string>
#include <unordered_map>

namespace pbus
{
	class ServiceRegistry
	{
		using ServiceMap = std::unordered_map<ServiceId, ServiceDescriptor, ServiceId::Hash, ServiceId::Equal>;

		static log::Logger			_log;

		ServiceMap					_services;

	public:
		void Add(const ServiceId & id, ServiceDescriptor && desc);
		const ServiceDescriptor * GetDescriptor(const ServiceId & id) const;
	};
}

#endif
