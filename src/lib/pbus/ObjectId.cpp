#include <pbus/ObjectId.h>

namespace pbus
{
	void ObjectId::Write(serialization::ISerializationStream & stream) const
	{
		Type.Write(stream);
		stream.Write(static_cast<s64>(Id));
	}

	ObjectId ObjectId::Read(ConstBuffer buffer)
	{
		ObjectId id(ClassId::Read(buffer), 0);
		return id;
	}
}
