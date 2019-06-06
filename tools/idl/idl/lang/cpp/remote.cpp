#include <pbus/idl/{{name}}.h>
{%- for dep in deps %}
#include <pbus/idl/{{dep}}.h>
{% endfor -%}
#include <pbus/MethodDescriptor.h>

namespace pbus { namespace idl
{
{% for method in methods %}
	{{method.rtype}} {{name}}::{{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %})
	{
		auto result = _session->Invoke<{{method.rtype}}>({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.name}}{% endfor %}).get_future();
		{% if method.rtype != "void" %}return result.get();{% else %}result.get();{% endif %}
	}
{% endfor %}

	void {{name}}::Register(const SessionPtr & session)
	{
{%- for dep in deps %}
		session->Register<{{dep}}>("{{dep}}");
{%- endfor %}
		session->Register<{{name}}>("{{name}}");
	}

}}
