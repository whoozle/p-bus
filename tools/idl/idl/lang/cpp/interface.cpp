#include <pbus/idl/I{{name}}.h>
#include <pbus/idl/{{name}}.h>
{%- for dep in deps %}
#include <pbus/idl/{{dep}}.h>
{% endfor -%}


namespace pbus { namespace idl
{
    pbus::ClassId I{{name}}::ClassId("{{name}}");

	void I{{name}}::RegisterProxy(Session & session)
	{
{%- for dep in deps %}
		session.Register<{{dep}}>(I{{dep}}::ClassId);
{%- endfor %}
		session.Register<{{name}}>(I{{name}}::ClassId);
	}

    I{{name}} * I{{name}}::CreateProxy(ObjectId id)
    { return new {{name}} (id); }
}}
