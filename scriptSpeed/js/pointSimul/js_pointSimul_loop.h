#ifndef SS_JS_POINT_SIMUL_LOOP_H
#define SS_JS_POINT_SIMUL_LOOP_H

#include "include/v8.h"

namespace js {
	namespace pointSimul {

		// Lua wrapper callbacks
		void updateVelocity(const FunctionCallbackInfo<Value>& args);
		void updatePoints(const FunctionCallbackInfo<Value>& args);
		void getPoint(const FunctionCallbackInfo<Value>& args);
		void getMouse(const FunctionCallbackInfo<Value>& args);
		void initSimul(const FunctionCallbackInfo<Value>& args);
		void draw(const FunctionCallbackInfo<Value>& args);
		void preDraw(const FunctionCallbackInfo<Value>& args);
		void postDraw(const FunctionCallbackInfo<Value>& args);
		void drawMouse(const FunctionCallbackInfo<Value>& args);
		void drawPoint(const FunctionCallbackInfo<Value>& args);
		void drawPointByCoords(const FunctionCallbackInfo<Value>& args);
		void cleanup(const FunctionCallbackInfo<Value>& args);
		void processInput(const FunctionCallbackInfo<Value>& args);

		// one function to register all wrapper callbacks
		void registerCallbacks(Local<ObjectTemplate> global, Isolate* i);
		int runScenario(const char* name, int cycleCount, int pointCount);
	}
}

#endif
