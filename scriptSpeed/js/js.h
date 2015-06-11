#ifndef SB_JS_H
#define SB_JS_H

#include "../scenarios/scenarios.h"
#include "include/v8.h"

using namespace v8;

namespace js {

	// main scenario methods
	int runExpression(int c, char** v);
	int runPointSimul(int c, char** v);
	int runCallback(int c, char** v);

	static const ::CallbackMap scenarioMap[] = {
		{ "expression", runExpression },
		{ "callback", runCallback },
		{ "pointSimul", runPointSimul }
	};

	// common util functions
	void close(v8::Isolate* isolate);
	Isolate* init();
	Local<Script> loadScriptFromFile(Isolate* isolate, const char* fileName);
	Handle<Function> getFunction(Isolate* isolate, Local<Context> context, const char* functionName);
}

#endif