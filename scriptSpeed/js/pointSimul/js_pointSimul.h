#ifndef SS_JS_POINT_SIMUL_H
#define SS_JS_POINT_SIMUL_H

#include "include/v8.h"

#include "../../scenario/pointSimul/pointSimul.h"

using namespace v8;

namespace js {
	namespace pointSimul
	{
		// common util functions
		void stepPoint(const FunctionCallbackInfo<Value>& args);
		Local<ObjectTemplate> createPointDefinition(Isolate* isolate);
		Local<ObjectTemplate> createMouseDefinition(Isolate* isolate);

		// experimental scenarios
		int loopCallback(int c, int p);
		int loopSetGetMax(int c, int p);
		int loopSetGetMin(int c, int p);

		// main scenarios
		int allScript(int c, int p);
		int allScriptMinCallback(int c, int p);
		int allNative(int c, int p);
		int scriptToNative(int c, int p);

		static const ::pointSimul::CallbackMap versionMap[] = {
			{ "allScript", allScript },
			{ "allNative", allNative },
			{ "allScriptMinCallback", allScriptMinCallback },
			{ "scriptToNative", scriptToNative },

			// experimental
			{ "loopCallback", loopCallback },
			{ "loopSetGetMax", loopSetGetMax },
			{ "loopSetGetMin", loopSetGetMin }
		};

	}
}

#endif
