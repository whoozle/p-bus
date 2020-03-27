#include <pbus/idl/core/CoreObject.h>

namespace pbus { namespace idl { namespace core
{

	CoreObject::~CoreObject()
	{ Session::Get().Release(_origin, _id); }

}}}
