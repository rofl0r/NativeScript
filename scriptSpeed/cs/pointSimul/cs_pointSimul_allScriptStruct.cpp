#include <mono/jit/jit.h>

#include "../../scenario/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include "../cs.h"

using namespace pointSimul;

namespace cs {
	namespace pointSimul {

		MonoDomain* domS;
		MonoMethod* scriptS;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptStructLoop(int pointIndex) {

			double f = SS_PS_FRICTION;
			void* args[3] = { &f, getPoint(pointIndex), getMouse() };
			mono_runtime_invoke(scriptS, NULL, args, NULL);
		}

		/*
		Run scenario variant
		*/
		int allScriptStruct(int c, int p)
		{
			domS = init();

			MonoAssembly* assembly = compileFile(domS, "PointSimulStruct.cs");
			if (!assembly)
			{
				printf("JIT compiled assembly failed to load\n");
				return 1;
			}

			scriptS = getMethod(assembly, "", "PointSimulStruct", "allScript", 3);

			int result = run(&allScriptStructLoop);
			
			// isolate can be disposed after handle scope is already deleted
			close(domS);

			return result;
		}
	}
}
