#include <pbus/Application.h>
#include <pbus/idl/system/IRandomGenerator.h>
#include <stdlib.h>
#include <math.h>

namespace pbus { namespace
{
	class RandomGenerator : public idl::system::IRandomGenerator
	{
		uint getInteger(uint max)
		{ return floor(1.0 * rand() / RAND_MAX * max); }
	};
}}

int main(int argc, char **argv)
{
	using namespace pbus;
	Application app(RandomGenerator::ClassId);
	app.RegisterService<RandomGenerator>();
	app.Run();
	return 0;
}
