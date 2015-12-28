#include <mono/jit/jit.h>

#include "../../scenario/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include "../cs.h"

using namespace pointSimul;

namespace cs {
	namespace pointSimul {

		typedef void(SS_CS_CALL *ScriptFunction)(double, MonoObject*, MonoObject*, MonoException**);

		MonoDomain* domN;
		MonoAssembly *assemblyN;
		MonoObject* monoMouseN;
		ScriptFunction methodThunkN;
		MonoException* exN;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void scriptToNativeLoop(int pointIndex) {
			
			MonoObject *p = createPoint(assemblyN, domN, pointIndex);
			methodThunkN(SS_PS_FRICTION, p, monoMouseN, &exN);
		}

		/*
		Callback method
		*/
		SS_CS_CALLBACK_EXPORT void stepPoint(MonoObject* p) {
			stepPoint(extractPoint(p));
		}

		/*
		Run scenario variant
		*/
		int scriptToNative(int c, int p)
		{
			domN = init();

			assemblyN = compileFile(domN, "PointSimul.cs");
			if (!assemblyN)
			{
				printf("JIT compiled assembly failed to load\n");
				return 1;
			}

			mono_add_internal_call("PointSimul::stepPoint(Point)", (void*)stepPoint);

			MonoMethod* script = getMethod(assemblyN, "", "PointSimul", "scriptToNative", 3);
			methodThunkN = (ScriptFunction)mono_method_get_unmanaged_thunk(script);

			definePointCallbacks();
			defineMouseCallbacks();
			monoMouseN = createMouse(assemblyN, domN);
			
			int result = run(&scriptToNativeLoop);
			
			// isolate can be disposed after handle scope is already deleted
			close(domN);

			return result;
		}
	}
}
