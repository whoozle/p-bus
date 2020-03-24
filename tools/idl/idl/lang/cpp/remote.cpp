#include <pbus/idl/{{name}}.h>
#include <pbus/MethodId.h>

namespace pbus { namespace idl
{
	namespace
	{
		ClassId classId("{{name}}");
		{% for method in methods %}MethodId method_{{method.name}}(classId, "{{method.name}}");{% endfor %}
	}

{% for method in methods %}
	{{method.rtype}} {{name}}::{{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %})
	{
		auto result = Session::Get().Invoke<{{method.rtype}}>(method_{{method.name}}{% for arg in method.args %}, {% if not method.static %}this, {% endif %}{{arg.name}}{% endfor %}).get_future();
		{% if method.rtype != "void" %}return result.get();{% else %}result.get();{% endif %}
	}
{% endfor %}

}}
