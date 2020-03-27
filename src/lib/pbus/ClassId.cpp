#include <pbus/ClassId.h>
#include <toolkit/serialization/Serializator.h>

namespace pbus
{

	void ClassId::Write(serialization::ISerializationStream & out) const
	{ serialization::Serialize(out, Name, Version); }

	ClassId ClassId::Read(ConstBuffer buffer)
	{ }

}
