#include <pbus/ObjectId.h>
#include <toolkit/serialization/Serializator.h>

namespace pbus
{
	void ObjectId::Write(serialization::ISerializationStream & out) const
	{ serialization::Serialize(out, Type, Id); }

	ObjectId ObjectId::Read(ConstBuffer buffer)
	{
		ObjectId id(ClassId::Read(buffer), 0);
		return id;
	}
}
