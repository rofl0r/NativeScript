#include "include/v8.h"

#include "../../scenario/pointSimul/pointSimul.h"
#include "js_pointSimul.h"
#include "../js.h"

using namespace pointSimul;

namespace js {
	namespace pointSimul {

		Isolate* isolateMC;
		Local<ObjectTemplate> pointClassMC;
		Local<ObjectTemplate> mouseClassMC;
		Local<Context> contextMC;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptMinCallbackLoop(int pointIndex) {

			// scope of local handles is bound to this function
			HandleScope handle_scope(isolateMC); 

			// create point parameter
			Local<Object> jsPoint = pointClassMC->NewInstance();
			jsPoint->SetInternalField(0, External::New(isolateMC, getPoint(pointIndex)));

			// create mouse parameter
			Local<Object> jsMouse = mouseClassMC->NewInstance();
			jsMouse->SetInternalField(0, External::New(isolateMC, getMouse()));

			// create friction parameter
			Handle<Value> friction = Number::New(isolateMC, SS_PS_FRICTION);

			// call the script function (3 arguments, ignore result)
			Handle<Value> argv[3] = { jsPoint, jsMouse, friction };
			Handle<Value> result = getFunction(isolateMC, contextMC, "allScriptMinCallback")->Call(contextMC->Global(), 3, argv);
		}

		/*
		Run scenario variant
		*/
		int allScriptMinCallback(int c, int p)
		{
			isolateMC = init();
			int result;
			{
				HandleScope handle_scope(isolateMC); // scope of local handles is bound to this block (stack allocated)

				contextMC = Context::New(isolateMC);
				Context::Scope context_scope(contextMC);

				Local<Script> script = loadScriptFromFile(isolateMC, "pointSimul.js");
				script->Run();

				pointClassMC = createPointDefinition(isolateMC);
				mouseClassMC = createMouseDefinition(isolateMC);

				result = run(&allScriptMinCallbackLoop);
			}
			
			// isolate can be disposed after handle scope is already deleted
			close(isolateMC);

			return result;
		}
	}
}
