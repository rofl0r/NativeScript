#ifndef SB_JS_H
#define SB_JS_H

#include "../scenarios/scenarios.h"
#include "include/v8.h"

namespace js {
	int runExpression(int c, char** v);
	int runPointSimul(int c, char** v);

	void close(v8::Isolate* isolate);
	v8::Isolate* init();

	static const ::CallbackMap scenarioMap[] = {
		{ "expression", &runExpression },
		{ "pointSimul", &runPointSimul }
	};
}

#endif