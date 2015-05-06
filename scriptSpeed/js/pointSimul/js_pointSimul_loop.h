#ifndef SB_JS_POINT_SIMUL_LOOP_H
#define SB_JS_POINT_SIMUL_LOOP_H

#include "include/v8.h"

namespace js {
	namespace pointSimul {

		// Lua wrapper callbacks
		void updateVelocity(const v8::FunctionCallbackInfo<v8::Value>& args);
		void updatePoints(const v8::FunctionCallbackInfo<v8::Value>& args);
		void getPoint(const v8::FunctionCallbackInfo<v8::Value>& args);
		void getMouse(const v8::FunctionCallbackInfo<v8::Value>& args);
		void initSimul(const v8::FunctionCallbackInfo<v8::Value>& args);
		void draw(const v8::FunctionCallbackInfo<v8::Value>& args);
		void preDraw(const v8::FunctionCallbackInfo<v8::Value>& args);
		void postDraw(const v8::FunctionCallbackInfo<v8::Value>& args);
		void drawMouse(const v8::FunctionCallbackInfo<v8::Value>& args);
		void drawPoint(const v8::FunctionCallbackInfo<v8::Value>& args);
		void drawPointByCoords(const v8::FunctionCallbackInfo<v8::Value>& args);
		void cleanup(const v8::FunctionCallbackInfo<v8::Value>& args);
		void processInput(const v8::FunctionCallbackInfo<v8::Value>& args);

		// one function to register all wrapper callbacks
		void registerCallbacks(v8::Local<v8::ObjectTemplate> global, v8::Isolate* i);
		int runScenario(char* name, int cycleCount, int pointCount);
	}
}

#endif
