#include <pbus/idl/{{name}}.h>
{%- for dep in deps %}
#include <pbus/idl/{{dep}}.h>
{% endfor -%}
#include <pbus/MethodId.h>

namespace pbus { namespace idl
{
	namespace
	{
		ServiceId serviceId("{{name}}");
		{% for method in methods %}MethodId method_{{method.name}}(serviceId, "{{method.name}}");{% endfor %}
	}

{% for method in methods %}
	{{method.rtype}} {{name}}::{{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %})
	{
		auto result = _session->Invoke<{{method.rtype}}>(serviceId, method_{{method.name}}{% for arg in method.args %}, {% if not method.static %}this, {% endif %}{{arg.name}}{% endfor %}).get_future();
		{% if method.rtype != "void" %}return result.get();{% else %}result.get();{% endif %}
	}
{% endfor %}

	void {{name}}::Register(const SessionPtr & session)
	{
{%- for dep in deps %}
		session->Register<{{dep}}>(ServiceId("{{dep}}"));
{%- endfor %}
		session->Register<{{name}}>(serviceId);
	}

}}
