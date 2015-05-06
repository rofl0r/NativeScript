#include "../../scenarios/pointSimul/pointSimul.h"
#include "js_pointSimul.h"
#include "include/v8.h"
#include "../js.h"

using namespace pointSimul;

namespace js {
	namespace pointSimul {

		// TODO: get rid of the globals
		Isolate* isolateS;
		Local<ObjectTemplate> pointClassS;
		Local<ObjectTemplate> mouseClassS;
		Local<Context> context;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptLoop(int pointIndex) {

			// scope of local handles is bound to this function
			HandleScope handle_scope(isolateS); 

			// create point parameter
			Local<Object> jsPoint = pointClassS->NewInstance();
			jsPoint->SetInternalField(0, External::New(isolateS, getPoint(pointIndex)));

			// create mouse parameter
			Local<Object> jsMouse = mouseClassS->NewInstance();
			jsMouse->SetInternalField(0, External::New(isolateS, getMouse()));

			// create friction parameter
			Handle<Value> friction = Number::New(isolateS, SB_PS_FRICTION);

			// call the script function (3 arguments, ignore result)
			Handle<Value> argv[3] = { jsPoint, jsMouse, friction };
			Handle<Value> result = getFunction(isolateS, context, "allScript")->Call(context->Global(), 3, argv);
		}

		/*
		Run scenario variant
		*/
		int allScript(int c, int p)
		{
			isolateS = init();
			int result;
			{
				HandleScope handle_scope(isolateS); // scope of local handles is bound to this block (stack allocated)

				context = Context::New(isolateS);
				Context::Scope context_scope(context);

				Local<Script> script = loadScriptFromFile(isolateS, "js/scripts/pointSimul.js");
				script->Run();

				pointClassS = createPointDefinition(isolateS);
				mouseClassS = createMouseDefinition(isolateS);

				result = run(&allScriptLoop);
			}
			
			// isolate can be disposed after handle scope is already deleted
			close(isolateS);

			return result;
		}
	}
}
