#include "../../scenarios/callback/callback.h"
#include "include/v8.h"
#include "../../measure.h"
#include "../js.h"

using namespace v8;

namespace js {
	namespace callback {

		typedef void(*jsCb)(v8::FunctionCallbackInfo<v8::Value> const&);

		void cb0(const v8::FunctionCallbackInfo<v8::Value>& args) { ::callback::processCallback(1); }
		void cb1(const v8::FunctionCallbackInfo<v8::Value>& args) {	::callback::processCallback(
			Local<Value>::Cast(args[0])->NumberValue());}
		void cb2(const v8::FunctionCallbackInfo<v8::Value>& args) {	::callback::processCallback(
			Local<Value>::Cast(args[0])->NumberValue()+Local<Value>::Cast(args[1])->NumberValue());}
		void cb3(const v8::FunctionCallbackInfo<v8::Value>& args) {	::callback::processCallback(
			Local<Value>::Cast(args[0])->NumberValue()+Local<Value>::Cast(args[1])->NumberValue()
			+Local<Value>::Cast(args[2])->NumberValue());}
		void cb4(const v8::FunctionCallbackInfo<v8::Value>& args) {	::callback::processCallback(
			Local<Value>::Cast(args[0])->NumberValue()+Local<Value>::Cast(args[1])->NumberValue()
			+Local<Value>::Cast(args[2])->NumberValue()+Local<Value>::Cast(args[3])->NumberValue());}

		jsCb fncs[] = { cb0, cb1, cb2, cb3, cb4 };
	}

	int runCallback(int c, char** v)
	{
		if (::callback::readArgs(c, v)) return 1;

		char source[40 + 20 + SB_C_PARAM_CALL_STRING_MAX_LENGTH]; // base string literal + long digits(64bits) + callback str max length
		sprintf(source, "function f(){for(var i=1;i<%d;i++){c(%s);}}",
			::callback::getCycleCount() + 1, ::callback::getParamCallString());

		Isolate* i = init();
		{
			HandleScope handle_scope(i); // scope of local handles is bound to this block (stack allocated)

			Local<ObjectTemplate> global = ObjectTemplate::New(i);
			global->Set(String::NewFromUtf8(i, "c"), FunctionTemplate::New(i, callback::fncs[::callback::getParamCount()]));

			Local<Context> context = Context::New(i, NULL, global);
			Context::Scope context_scope(context);

			Local<Script> script = Script::Compile(String::NewFromUtf8(i, source));
			script->Run();
			
			// prepare parameters
			::callback::readArgs(c, v);
			Handle<Value> cycles = Number::New(i, ::callback::getCycleCount());
			Handle<Function> f = getFunction(i, context, "f");
			Local<Object> g = context->Global();

			measure::cpuStart();
			f->Call(g, 0, 0);
			measure::cpuStop();

			measure::cpuDisplayResults();
			::callback::validateResults();
		}
		close(i);

		return 0;
	}

}
