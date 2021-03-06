#include <mono/jit/jit.h>

#include "../../scenario/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include "../cs.h"

using namespace pointSimul;

namespace cs {
	namespace pointSimul {

		MonoMethod* scriptNS;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void scriptToNativeLoopStruct(int pointIndex) {
			double f = SS_PS_FRICTION;
			void* args[3] = { &f, getPoint(pointIndex), getMouse() };
			mono_runtime_invoke(scriptNS, NULL, args, NULL);
		}

		/*
		Callback method
		*/
		SS_CS_CALLBACK_EXPORT void stepPointStruct(struct point* p) {
			stepPoint(p);
		}

		/*
		Run scenario variant
		*/
		int scriptToNativeStruct(int c, int p)
		{
			MonoDomain* domMS = init();

			MonoAssembly* assembly = compileFile(domMS, "PointSimulStruct.cs");
			if (!assembly)
			{
				printf("JIT compiled assembly failed to load\n");
				return 1;
			}

			mono_add_internal_call("PointSimulStruct::stepPoint(PointStruct&)", (void*)stepPointStruct);

			scriptNS = getMethod(assembly, "", "PointSimulStruct", "scriptToNative", 3);

			int result = run(&scriptToNativeLoopStruct);

			// isolate can be disposed after handle scope is already deleted
			close(domMS);

			return result;
		}
	}
}
