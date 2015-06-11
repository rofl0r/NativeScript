#include "../../scenarios/callback/callback.h"
#include "include/v8.h"
#include "../../measure.h"
#include "../js.h"

using namespace v8;

namespace js {
	namespace callback {
		void callbackNative(const v8::FunctionCallbackInfo<v8::Value>& args) {
			::callback::executeCallbackBody();
		}
	}

	int runCallback(int c, char** v)
	{
		Isolate* i = init();
		{
			HandleScope handle_scope(i); // scope of local handles is bound to this block (stack allocated)

			Local<ObjectTemplate> global = ObjectTemplate::New(i);
			global->Set(String::NewFromUtf8(i, "callbackNative"), FunctionTemplate::New(i, callback::callbackNative));

			Local<Context> context = Context::New(i, NULL, global);
			Context::Scope context_scope(context);

			Local<Script> script = loadScriptFromFile(i, "callback.js");
			script->Run();
			
			// prepare parameters
			Handle<Value> cycles = Number::New(i, ::callback::getCycleCount(c, v));
			Handle<Value> argv[1] = {cycles};
			Handle<Function> f = getFunction(i, context, "simpleCallback");
			Local<Object> g = context->Global();

			measure::cpuStart();
			f->Call(g, 1, argv);
			measure::cpuStop();

			::callback::displayResults();
			measure::cpuDisplayResults();
		}
		close(i);

		return 0;
	}

}
