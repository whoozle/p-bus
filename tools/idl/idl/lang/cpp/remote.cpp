#include <pbus/idl/{{package|replace(".", "/")}}/{{name}}.h>
#include <pbus/MethodId.h>

namespace pbus { namespace idl{%- for pc in package_components %} { namespace {{pc}} {%- endfor %}
{
	namespace
	{
		{% for method in methods %}MethodId method_{{method.name}}(I{{name}}::ClassId, "{{method.name}}");{% endfor %}
	}

{% for method in methods %}
	{{method.rtype}} {{name}}::{{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %})
	{
		auto result = Session::Get().Invoke<{{method.rtype}}>(GetOrigin(), GetId(), method_{{method.name}}{% for arg in method.args %}, {{arg.name}}{% endfor %}).get_future();
		{% if method.rtype != "void" %}return result.get();{% else %}result.get();{% endif %}
	}
{% endfor %}

{% for pc in package_components %} } {%- endfor %}
}}
