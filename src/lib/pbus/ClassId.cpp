#include <pbus/ClassId.h>

namespace pbus
{

	void ClassId::Write(serialization::ISerializationStream & stream) const
	{
		stream.Write(Name);
		stream.Write(static_cast<s64>(Version));
	}

	ClassId ClassId::Read(ConstBuffer buffer)
	{ }

}
