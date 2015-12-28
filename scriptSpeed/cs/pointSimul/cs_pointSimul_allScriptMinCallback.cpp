#include <mono/jit/jit.h>

#include "../../scenario/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include "../cs.h"

using namespace pointSimul;

namespace cs {
	namespace pointSimul {

		typedef void(SS_CS_CALL *ScriptFunction)(double, MonoObject*, MonoObject*, MonoException**);

		MonoDomain* domM;
		MonoAssembly *assemblyM;
		MonoObject* monoMouseM;
		ScriptFunction methodThunkM;
		MonoException* exM;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptMinCallbackLoop(int pointIndex) {
			
			MonoObject *p = createPoint(assemblyM, domM, pointIndex);
			methodThunkM(SS_PS_FRICTION, p, monoMouseM, &exM);
		}

		/*
		Run scenario variant
		*/
		int allScriptMinCallback(int c, int p)
		{
			domM = init();

			assemblyM = compileFile(domM, "PointSimul.cs");
			if (!assemblyM)
			{
				printf("JIT compiled assembly failed to load\n");
				return 1;
			}

			MonoMethod* script = getMethod(assemblyM, "", "PointSimul", "allScriptMinCallback", 3);
			methodThunkM = (ScriptFunction)mono_method_get_unmanaged_thunk(script);

			definePointCallbacks();
			defineMouseCallbacks();
			monoMouseM = createMouse(assemblyM, domM);
			
			int result = run(&allScriptMinCallbackLoop);
			
			// isolate can be disposed after handle scope is already deleted
			close(domM);

			return result;
		}
	}
}
