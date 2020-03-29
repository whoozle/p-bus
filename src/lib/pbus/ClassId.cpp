#include <pbus/ClassId.h>
#include <toolkit/serialization/Serializator.h>

namespace pbus
{

	void ClassId::Write(serialization::ISerializationStream & out) const
	{ serialization::Serialize(out, Name, Version); }

	ClassId ClassId::FromString(const std::string & str)
	{
		auto atPos = str.find('@');
		if (atPos == str.npos)
			throw Exception("no version in service id");

		return ServiceId(str.substr(0, atPos), std::stoi(str.substr(atPos + 1)));
	}

}
