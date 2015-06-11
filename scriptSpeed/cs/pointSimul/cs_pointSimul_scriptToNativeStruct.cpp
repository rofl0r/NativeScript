#include "../../scenarios/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "../cs.h"

using namespace pointSimul;

namespace cs {
	namespace pointSimul {

		MonoMethod* scriptNS;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void scriptToNativeLoopStruct(int pointIndex) {
			double f = SB_PS_FRICTION;
			void* args[3] = { &f, getPoint(pointIndex), getMouse() };
			mono_runtime_invoke(scriptNS, NULL, args, NULL);
		}

		/*
		Callback method
		*/
		__declspec(dllexport) void stepPointStruct(struct point* p) {
			stepPoint(p);
		}

		/*
		Run scenario variant
		*/
		int scriptToNativeStruct(int c, int p)
		{
			MonoDomain* domMS = init();

			MonoAssembly* assembly = compileSource(domMS, "PointSimulStruct.cs");
			if (!assembly)
			{
				printf("JIT compiled assembly failed to load\n");
				return 1;
			}

			mono_add_internal_call("PointSimulStruct::stepPoint(PointStruct&)", stepPointStruct);

			scriptNS = getMethod(assembly, "", "PointSimulStruct", "scriptToNative", 3);

			int result = run(&scriptToNativeLoopStruct);

			// isolate can be disposed after handle scope is already deleted
			close(domMS);

			return result;
		}
	}
}
