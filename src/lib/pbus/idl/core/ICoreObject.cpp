#include <pbus/idl/core/ICoreObject.h>
namespace pbus { namespace idl { namespace core
{
    pbus::ClassId ICoreObject::ClassId("core.CoreObject");

	void ICoreObject::__pbus__invoke(serialization::ISerializationStream & resultStream, const std::string & method, ConstBuffer argsData)
	{ throw Exception("invoking unknown method " + method); }
}}}
