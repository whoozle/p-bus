#include <pbus/ObjectId.h>
#include <toolkit/serialization/Serializator.h>

namespace pbus
{
	void ObjectId::Write(serialization::ISerializationStream & out) const
	{ serialization::Serialize(out, Type, Id); }
}
