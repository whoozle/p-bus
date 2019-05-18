#include <pbus/idl/{{name}}Remote.h>
{% for dep in deps %}
#include <pbus/idl/{{dep}}Remote.h>
{% endfor %}

namespace pbus { namespace idl
{
{% for method in methods %}
	{{method.rtype}} {{name}}Remote::{{method.name}}({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.type}} {{arg.name}}{% endfor %})
	{
		auto result = _session->Invoke<{{method.rtype}}>({% for arg in method.args %}{% if not loop.first %}, {% endif %}{{arg.name}}{% endfor %});
		{% if method.rtype != "void" %}return result.get();{% else %}result.get();{% endif %}
	}
{% endfor %}

	void {{name}}Remote::Register(const SessionPtr & session)
	{
{%- for dep in deps %}
		session->Register<{{dep}}Remote>("{{dep}}");
{%- endfor %}
		session->Register<{{name}}Remote>("{{name}}");
	}

}}
