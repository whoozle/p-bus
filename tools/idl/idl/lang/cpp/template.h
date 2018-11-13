#ifndef PBUS_GENERATED_IDL_{{name|upper}}_H
#define PBUS_GENERATED_IDL_{{name|upper}}_H

namespace pbus { namespace idl
{

	struct {{name}} {% if base %} : {{base}} {% endif %}
	{
		{% for method in methods %}
		{{method.rtype}} {{method.name}} ( {% for arg in method.args %} {% if not loop.first %}, {% endif %} {{arg.type}} {{arg.name}} {% endfor %} );
		{% endfor %}
	};

}}

#endif
