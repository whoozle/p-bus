#include <pbus/idl/{{package|replace(".", "/")}}/I{{name}}.h>
#include <pbus/idl/{{package|replace(".", "/")}}/{{name}}.h>
{%- for deppackage, depns, dep in deps %}
#include <pbus/idl/{{deppackage | replace(".", "/")}}/{{dep}}.h>
{%- endfor %}

namespace pbus { namespace idl{%- for pc in package_components %} { namespace {{pc}} {%- endfor %}
{
    pbus::ClassId I{{name}}::ClassId("{{package}}.{{name}}");

	void I{{name}}::RegisterProxy(Session & session)
	{
{%- for deppackage, depns, dep in deps %}
		session.RegisterProxy<{{depns}}::{{dep}}>(I{{dep}}::ClassId);
{%- endfor %}
		session.RegisterProxy<{{name}}>(I{{name}}::ClassId);
	}

    I{{name}} * I{{name}}::CreateProxy(ObjectId id)
    { return new {{name}} (id); }

{% for pc in package_components %} } {%- endfor %}
}}
