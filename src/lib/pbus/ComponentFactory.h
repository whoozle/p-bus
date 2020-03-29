#ifndef PBUS_COMPONENT_FACTORY_H
#define PBUS_COMPONENT_FACTORY_H

#include <pbus/idl/core/ICoreObject.h>
#include <pbus/ObjectId.h>
#include <atomic>

namespace pbus
{
	struct IComponentFactory
	{
		virtual ~IComponentFactory() = default;
		virtual idl::core::ICoreObject * CreateRoot() = 0;
		virtual idl::core::ICoreObject * Create() = 0;
	};
	TOOLKIT_DECLARE_PTR(IComponentFactory);

	template <typename InterfaceType>
	struct ITypedComponentFactory : public IComponentFactory
	{
		virtual InterfaceType * CreateRoot() = 0;
		virtual InterfaceType * Create() = 0;
	};

	template <typename Component, typename InterfaceType = typename Component::InterfaceType>
	class ComponentFactory : public ITypedComponentFactory<InterfaceType>
	{
		ServiceId						_serviceId;
		std::atomic<ObjectId::IdType>	_nextObjectId;

	public:
		ComponentFactory(ServiceId serviceId): _serviceId(serviceId), _nextObjectId(1)
		{ }

		InterfaceType * CreateRoot() override
		{ return new Component(_serviceId, ObjectId(InterfaceType::ClassId, 0)); }

		InterfaceType * Create() override
		{
			auto id = _nextObjectId.fetch_add(1, std::memory_order_relaxed);
			return new Component(_serviceId, ObjectId(InterfaceType::ClassId, id));
		}
	};

	struct IServiceFactory
	{
		virtual ~IServiceFactory() = default;
		virtual idl::core::ICoreObjectPtr Get() = 0;
	};
	TOOLKIT_DECLARE_PTR(IServiceFactory);

}

#endif
