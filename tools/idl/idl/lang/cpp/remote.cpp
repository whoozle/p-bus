#include <pbus/idl/{{name}}.h>

namespace pbus { namespace idl
{
{% for method in methods %}
	{{method.rtype}} {{name}}Remote::{{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %})
	{ }
{% endfor %}
}}
