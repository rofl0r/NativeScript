#ifndef SB_JS_POINT_SIMUL_H
#define SB_JS_POINT_SIMUL_H

#include "include/v8.h"

using namespace v8;

namespace js {
	int runPointSimul(int c, char** v);

	namespace pointSimul
	{
		void stepPoint(const v8::FunctionCallbackInfo<v8::Value>& args);
		Local<ObjectTemplate> createPointDefinition(Isolate* isolate);
		Local<ObjectTemplate> createMouseDefinition(Isolate* isolate);
		Local<Script> loadScriptFromFile(Isolate* isolate, const char* fileName);
		Handle<Function> getFunction(Isolate* isolate, Local<Context> context, const char* functionName);

		int allScript(int c, int p);
		int allScriptMinCallback(int c, int p);
		int allNative(int c, int p);
		int scriptToNative(int c, int p);
		int loopCallback(int c, int p);
		int loopSetGetMax(int c, int p);
		int loopSetGetMin(int c, int p);
	}
}

#endif