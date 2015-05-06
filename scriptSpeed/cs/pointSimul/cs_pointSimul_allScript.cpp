#include "../../scenarios/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "../cs.h"

using namespace pointSimul;

namespace cs {
	namespace pointSimul {

		MonoDomain* dom;
		MonoAssembly *assembly;
		MonoObject* mouse;
		MonoMethod* script;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptLoop(int pointIndex) {
			
			MonoObject* point = createPoint(assembly, dom, getPoint(pointIndex));
			
			double friction = SB_PS_FRICTION;
			void *params[] = { point, mouse, &friction };
			callMethod(script, params);
		}

		/*
		Run scenario variant
		*/
		int allScript(int c, int p)
		{
			dom = init();

			assembly = compileSource(dom, "cs/scripts/PointSimul.cs");
			if (!assembly)
			{
				printf("JIT compiled assembly failed to load\n");
				return 1;
			}
			script = getMethod(assembly, "", "PointSimul", "allScript", 3);
			
			definePointCallbacks();
			defineMouseCallbacks();
			mouse = createMouse(assembly, dom);
			
			int result = run(&allScriptLoop);
			
			// isolate can be disposed after handle scope is already deleted
			close(dom);

			return result;
		}
	}
}
