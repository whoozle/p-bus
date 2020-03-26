#ifndef PBUS_APPLICATION_H
#define PBUS_APPLICATION_H

#include <pbus/ClassId.h>
#include <pbus/LocalBus.h>
#include <pbus/Session.h>

namespace pbus
{
	class Application
	{
		LocalBus  _bus;
	public:
		Application(const ClassId & classId);

		template<typename ClassType>
		void RegisterClass(const ClassId & classId)
		{
			auto & session = Session::Get();
			session.RegisterProxy<ClassType>(classId, true);
		}

		void Run();
	};
}

#endif
