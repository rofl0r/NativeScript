#include <mono/jit/jit.h>

#include "../../scenario/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include "../cs.h"

using namespace pointSimul;

namespace cs {
	namespace pointSimul {

		typedef void(__stdcall *ScriptFunction)(double, MonoObject*, MonoObject*, MonoException**);

		MonoDomain* dom;
		MonoAssembly *assembly;
		MonoObject* monoMouse;
		ScriptFunction methodThunk;
		MonoException* ex;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptLoop(int pointIndex) {
			
			MonoObject *p = createPoint(assembly, dom, pointIndex);
			methodThunk(SS_PS_FRICTION, p, monoMouse, &ex);
		}

		/*
		Run scenario variant
		*/
		int allScript(int c, int p)
		{
			dom = init();

			assembly = compileFile(dom, "PointSimul.cs");
			if (!assembly)
			{
				printf("JIT compiled assembly failed to load\n");
				return 1;
			}

			MonoMethod* script = getMethod(assembly, "", "PointSimul", "allScript", 3);
			methodThunk = (ScriptFunction)mono_method_get_unmanaged_thunk(script);

			definePointCallbacks();
			defineMouseCallbacks();
			monoMouse = createMouse(assembly, dom);
			
			int result = run(&allScriptLoop);
			
			// isolate can be disposed after handle scope is already deleted
			close(dom);

			return result;
		}
	}
}
