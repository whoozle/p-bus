#ifndef PBUS_SESSION_H
#define PBUS_SESSION_H

#include <toolkit/core/types.h>
#include <pbus/idl/ICoreObject.h>
#include <pbus/idl/IService.h>
#include <pbus/ServiceId.h>
#include <memory>
#include <unordered_map>

namespace pbus
{
	class Session;
	DECLARE_PTR(Session);

	struct IComponentFactory
	{
		virtual ~IComponentFactory() = default;
		virtual idl::ICoreObject * Create(const SessionPtr & session) = 0;
	};
	DECLARE_PTR(IComponentFactory);

	template<typename Component>
	struct ComponentFactory : public IComponentFactory
	{
		idl::ICoreObject * Create(const SessionPtr & session) override
		{ return new Component(session); }
	};


	class Session final : public std::enable_shared_from_this<Session>
	{
		std::unordered_map<std::string, IComponentFactoryPtr> _factories;

	public:
		SessionPtr GetPointer()
		{ return shared_from_this();}

		template<typename Component>
		void Register(const std::string &name)
		{
			auto it = _factories.find(name);
			if (it != _factories.end())
				return;
			_factories[name] = std::make_shared<ComponentFactory<Component>>();
		}

		template<typename InterfaceType>
		std::shared_ptr<InterfaceType> GetService(ServiceId serviceId)
		{
			return nullptr;
		}
	};
}


#endif
