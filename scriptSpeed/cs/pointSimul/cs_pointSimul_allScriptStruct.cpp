#include "../../scenario/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "../cs.h"

using namespace pointSimul;

namespace cs {
	namespace pointSimul {

		/*typedef MonoObject*(__stdcall *ScriptFunctionS)(double, MonoObject*, MonoObject*, MonoException**);

		ScriptFunctionS methodThunkS;
		MonoClass* pointClass;
		MonoClass* mouseClass;
		MonoException* exS;*/
		MonoDomain* domS;
		MonoMethod* scriptS;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptStructLoop(int pointIndex) {

			// TODO thunk needs the script to return point (so far the best I figured out) and is slower (probably because of boxing)
			/*point* p = getPoint(pointIndex);
			MonoObject * boxedPoint = mono_value_box(domS, pointClass, p);
			MonoObject * boxedMouse = mono_value_box(domS, mouseClass, getMouse());
			point* pm = (point*)mono_object_unbox(methodThunkS(SB_PS_FRICTION, boxedPoint, boxedMouse, &exS));
			p->x = pm->x;
			p->y = pm->y;
			p->xVelocity = pm->xVelocity;
			p->yVelocity = pm->yVelocity;*/

			double f = SB_PS_FRICTION;
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

			/*methodThunkS = (ScriptFunctionS)mono_method_get_unmanaged_thunk(scriptS);
			MonoImage *image = mono_assembly_get_image(assembly);
			pointClass = mono_class_from_name(image, "", "PointStruct");
			mouseClass = mono_class_from_name(image, "", "MouseStruct");*/
			
			int result = run(&allScriptStructLoop);
			
			// isolate can be disposed after handle scope is already deleted
			close(domS);

			return result;
		}
	}
}
