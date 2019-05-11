#ifndef PBUS_GENERATED_IDL_{{name|upper}}_H
#define PBUS_GENERATED_IDL_{{name|upper}}_H

#include <pbus/idl/I{{name}}.h>
#include <pbus/idl/{{base}}Remote.h>

namespace pbus { namespace idl
{

	struct {{name}}Remote : public {{base}}Remote, public virtual I{{name}}
	{
		using {{base}}Remote::{{base}}Remote;
		{%- for method in methods %}
		{{method.rtype}} {{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %}) override;
		{%- endfor %}

		static void Register(const SessionPtr & ptr);
	};

}}

#endif