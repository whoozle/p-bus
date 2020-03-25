#ifndef PBUS_APPLICATION_H
#define PBUS_APPLICATION_H

#include <pbus/ClassId.h>
#include <pbus/LocalBus.h>

namespace pbus
{
	class Application
	{
		LocalBus  _bus;
	public:
		Application(const ClassId & classId);

		void Run();
	};
}

#endif
