#ifndef PBUS_GENERATED_IDL_I{{name|upper}}_H
#define PBUS_GENERATED_IDL_I{{name|upper}}_H

#include <pbus/idl/types.h>
{%- for dep in deps %}
#include <pbus/idl/I{{dep}}.h>
{%- endfor %}
#include <pbus/Session.h>

namespace pbus { namespace idl
{

	struct I{{name}} : virtual I{{base}}
	{
		virtual ~I{{name}}() = default;
		{%- for method in methods %}
		virtual {{method.rtype}} {{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %}) = 0;
		{%- endfor %}
	};

}}

#endif
