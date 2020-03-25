#ifndef PBUS_SERVICE_H
#define PBUS_SERVICE_H

#include <pbus/LocalBus.h>
#include <system/servicemanager/ServiceRegistry.h>

namespace pbus
{
	class Service
	{
		ServiceDescriptor	_descriptor;
		LocalBus 			_bus;
	public:
		Service(const ServiceDescriptor & descriptor);
	};
}

#endif

