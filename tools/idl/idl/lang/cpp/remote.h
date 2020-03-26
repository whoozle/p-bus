#ifndef PBUS_GENERATED_IDL_{{name|upper}}_H
#define PBUS_GENERATED_IDL_{{name|upper}}_H

#include <pbus/idl/{{package|replace(".", "/")}}/I{{name}}.h>
#include <pbus/idl/{{base_namespace|replace(".", "/")}}/{{base}}.h>

namespace pbus { namespace idl{%- for pc in package_components %} { namespace {{pc}} {%- endfor %}
{

	struct {{name}} :
		public {{base_namespace}}::{{base}},
		public virtual I{{name}},
		public virtual core::IRemoteObject
	{
		using {{base}}::{{base}};
		{%- for method in methods %}
		{{method.rtype}} {{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %}) override;
		{%- endfor %}
	};

{% for pc in package_components %} } {%- endfor %}

}}

#endif
