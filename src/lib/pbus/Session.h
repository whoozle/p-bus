#ifndef PBUS_SESSION_H
#define PBUS_SESSION_H

#include <toolkit/core/types.h>
#include <pbus/idl/ICoreObject.h>
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
	struct ComponentFactory
	{
		idl::ICoreObject * Create(const SessionPtr & session) override
		{ return nullptr; }
	};


	class Session
	{
		std::unordered_map<std::string, IComponentFactoryPtr> _factories;

	public:
		template<typename Component>
		void Register(const std::string &name)
		{ }
	};
}


#endif
