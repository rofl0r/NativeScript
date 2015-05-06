#ifndef SB_CS_POINT_SIMUL_H
#define SB_CS_POINT_SIMUL_H

#include <mono/jit/jit.h>
#include "../../scenarios/pointSimul/pointSimul.h"

using namespace pointSimul;

namespace cs {
	int runPointSimul(int c, char** v);

	namespace pointSimul
	{
		// callbacks
		__declspec(dllexport) void stepPoint(MonoObject* point);

		void definePointCallbacks();
		void defineMouseCallbacks();
		MonoObject* createPoint(MonoAssembly* assembly, MonoDomain* domain, point* p);
		point* extractPoint(MonoObject* pointObject);
		MonoObject* createMouse(MonoAssembly* assembly, MonoDomain* domain);

		// scenario methods
		int allScript(int c, int p);
	}
}

#endif