#include "../../scenario/pointSimul/pointSimul.h"
#include "js_pointSimul.h"
#include "include/v8.h"
#include "../js.h"

using namespace pointSimul;

namespace js {
	namespace pointSimul {

		// TODO: get rid of the globals
		Isolate* isolateTN;
		Local<ObjectTemplate> pointClassTN;
		Local<ObjectTemplate> mouseClassTN;
		Local<Context> contextTN;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void scriptToNativeLoop(int pointIndex) {

			// scope of local handles is bound to this function
			HandleScope handle_scope(isolateTN); 

			// create point parameter
			Local<Object> jsPoint = pointClassTN->NewInstance();
			jsPoint->SetInternalField(0, External::New(isolateTN, getPoint(pointIndex)));

			// create mouse parameter
			Local<Object> jsMouse = mouseClassTN->NewInstance();
			jsMouse->SetInternalField(0, External::New(isolateTN, getMouse()));

			// create friction parameter
			Handle<Value> friction = Number::New(isolateTN, SB_PS_FRICTION);

			// call the script function (3 arguments, ignore result)
			Handle<Value> argv[3] = { jsPoint, jsMouse, friction };
			Handle<Value> result = getFunction(isolateTN, contextTN, "allScript")->Call(contextTN->Global(), 3, argv);
		}

		/*
		Run scenario variant
		*/
		int scriptToNative(int c, int p)
		{
			isolateTN = init();
			int result;
			{
				HandleScope handle_scope(isolateTN); // scope of local handles is bound to this block (stack allocated)

				Local<ObjectTemplate> global = ObjectTemplate::New(isolateTN);
				global->Set(String::NewFromUtf8(isolateTN, "stepPoint"), FunctionTemplate::New(isolateTN, stepPoint));

				contextTN = Context::New(isolateTN, NULL, global);
				Context::Scope context_scope(contextTN);

				Local<Script> script = loadScriptFromFile(isolateTN, "pointSimul.js");
				script->Run();

				pointClassTN = createPointDefinition(isolateTN);
				mouseClassTN = createMouseDefinition(isolateTN);

				result = run(&scriptToNativeLoop);
			}
			
			// isolate can be disposed after handle scope is already deleted
			close(isolateTN);

			return result;
		}
	}
}
