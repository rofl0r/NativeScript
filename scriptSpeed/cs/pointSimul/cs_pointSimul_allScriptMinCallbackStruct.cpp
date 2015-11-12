#include "../../scenarios/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "../cs.h"

using namespace pointSimul;

namespace cs {
	namespace pointSimul {

		MonoMethod* scriptMS;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptMinCallbackLoopStruct(int pointIndex) {
			double f = SB_PS_FRICTION;
			void* args[3] = { &f, getPoint(pointIndex), getMouse() };
			mono_runtime_invoke(scriptMS, NULL, args, NULL);
		}

		/*
		Run scenario variant
		*/
		int allScriptMinCallbackStruct(int c, int p)
		{
			MonoDomain* domMS = init();

			MonoAssembly* assembly = compileFile(domMS, "PointSimulStruct.cs");
			if (!assembly)
			{
				printf("JIT compiled assembly failed to load\n");
				return 1;
			}

			scriptMS = getMethod(assembly, "", "PointSimulStruct", "allScriptMinCallback", 3);

			int result = run(&allScriptMinCallbackLoopStruct);

			// isolate can be disposed after handle scope is already deleted
			close(domMS);

			return result;
		}
	}
}
