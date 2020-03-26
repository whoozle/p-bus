#ifndef PBUS_GENERATED_IDL_I{{name|upper}}_H
#define PBUS_GENERATED_IDL_I{{name|upper}}_H

#include <pbus/idl/types.h>
{%- for deppackage, depns, dep in deps %}
#include <pbus/idl/{{deppackage | replace(".", "/")}}/I{{dep}}.h>
{%- endfor %}
#include <memory>

namespace pbus { class Session; namespace idl{%- for pc in package_components %} { namespace {{pc}} {%- endfor %}
{

	struct I{{name}} : virtual {{base_namespace}}::I{{base}}
	{
		virtual ~I{{name}}() = default;
		{%- for method in methods %}
		virtual {{method.rtype}} {{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %}) = 0;
		{%- endfor %}

		static pbus::ClassId ClassId;

		static void RegisterProxy(Session & session);
		static I{{name}} * CreateProxy(ObjectId id);
	};
	TOOLKIT_DECLARE_PTR(I{{name}});
	TOOLKIT_DECLARE_CONST_PTR(I{{name}});
	TOOLKIT_DECLARE_WEAK_PTR(I{{name}});
	TOOLKIT_DECLARE_CONST_WEAK_PTR(I{{name}});

{% for pc in package_components %} } {%- endfor %}
}}

#endif
