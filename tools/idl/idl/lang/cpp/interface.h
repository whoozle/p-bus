#ifndef PBUS_GENERATED_IDL_I{{name|upper}}_H
#define PBUS_GENERATED_IDL_I{{name|upper}}_H

{% if base %}#include <pbus/idl/I{{base}}.h>{% endif %}

namespace pbus { namespace idl
{

	struct I{{name}} {% if base %} : I{{base}} {% endif %}
	{
		virtual ~I{{name}}() = default;
		{%- for method in methods %}
		virtual {{method.rtype}} {{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %}) = 0;
		{%- endfor %}
	};

}}

#endif
