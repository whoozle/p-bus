#include <pbus/idl/{{package|replace(".", "/")}}/I{{name}}.h>
#include <pbus/idl/{{package|replace(".", "/")}}/{{name}}.h>
{%- for deppackage, depns, dep in deps %}
#include <pbus/idl/{{deppackage | replace(".", "/")}}/{{dep}}.h>
{%- endfor %}
#include <toolkit/serialization/Serializator.h>
#include <toolkit/serialization/bson/InputStream.h>

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

	void I{{name}}::__pbus__invoke(serialization::ISerializationStream & resultStream, const std::string & method, ConstBuffer argsData)
	{
		{% if methods -%}
		size_t offset = 0; (void)offset;
		{%- endif -%}
		{%- for method in methods %}
		if (method == "{{method.name}}") {
			{% for arg in method.args -%}
			auto arg{{loop.index}} = serialization::bson::ReadSingleValue<std::decay<{{arg.type}}>::type>(argsData, offset);
			{% endfor -%}
			{% if method.rtype != "void" -%}serialization::Serialize(resultStream, {% endif -%}
			{{method.name}}(
				{%- for i in range(method.args | length) -%}
					{%- if i > 0 -%}, {% endif -%}
					arg{{i + 1}}
				{%- endfor -%}
			)
			{%- if method.rtype != "void" %}){% else %};
			serialization::Serialize(resultStream, serialization::Undefined()){% endif -%};
		} else
		{%- endfor %}
			I{{base}}::__pbus__invoke(resultStream, method, argsData);
	}

    I{{name}} * I{{name}}::CreateProxy(const ServiceId &origin, const ObjectId &id)
    { return new {{name}} (origin, id); }

{% for pc in package_components %} } {%- endfor %}
}}
