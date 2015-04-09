/* JavaScript results in ms:
- script starting overhead, then faster than lua
- calls to c cheap
         Debug Wall   Debug CPU   Release Wall   Release CPU
Native:                                 33            31 
ScToNa:                               1500          1550
ScMinC:                               2700          2700
Script:                               2250          2200
*/

#include "gameGlobal.h"
#include "../shared/settings.h"
#include "../shared/game.h"
#include "include/v8.h"
#include "include/libplatform/libplatform.h"
#include <SDL.h>
#include "Windows.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
using namespace v8;

////////////////////////////////////////// Settings /////////////////////////////////////////
// js callbacks

void stepPoint(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Local<Object> self = Local<Object>::Cast(args[0]);
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	stepPoint(static_cast<point*>(wrap->Value()));
}


// Js wrapper functions
void setPointX(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<point*>(wrap->Value())->x = value->NumberValue();
}

void getPointX(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<point*>(wrap->Value())->x);
}
void setPointY(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<point*>(wrap->Value())->y = value->NumberValue();
}

void getPointY(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<point*>(wrap->Value())->y);
}
void setVelocityX(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<point*>(wrap->Value())->xVelocity = value->NumberValue();
}

void getVelocityX(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<point*>(wrap->Value())->xVelocity);
}
void setVelocityY(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<point*>(wrap->Value())->yVelocity = value->NumberValue();
}

void getVelocityY(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<point*>(wrap->Value())->yVelocity);
}
void setMouseX(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<mouse*>(wrap->Value())->x = value->Int32Value();
}
void getMouseX(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<mouse*>(wrap->Value())->x);
}
void setMouseY(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<mouse*>(wrap->Value())->y = value->Int32Value();
}
void getMouseY(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<mouse*>(wrap->Value())->y);
}

// Game skeleton

int gameGlobal() {
	initMeasurement();

	initDrawing();

	initGame();

	V8::InitializeICU();
	Platform* platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();

	// initialize js
	// Create a new Isolate and make it the current one.
	Isolate* isolate = Isolate::New();
	Isolate::Scope isolate_scope(isolate);

	// Create a stack-allocated handle scope.
	HandleScope handle_scope(isolate);
	
	// Create point object wrapper and global object
	Local<ObjectTemplate> global_templ = ObjectTemplate::New(isolate);
	global_templ->Set(String::NewFromUtf8(isolate, "stepPoint"), FunctionTemplate::New(isolate, stepPoint));
	Local<Context> context = Context::New(isolate, NULL, global_templ);
	Context::Scope context_scope(context);
	Local<Object> global = Local<Object>::Cast(context->Global()->GetPrototype());
	
	Local<ObjectTemplate> point_templ = ObjectTemplate::New(isolate);
	point_templ->SetInternalFieldCount(1);
	point_templ->SetAccessor(String::NewFromUtf8(isolate, "x"), getPointX, setPointX);
	point_templ->SetAccessor(String::NewFromUtf8(isolate, "y"), getPointY, setPointY);
	point_templ->SetAccessor(String::NewFromUtf8(isolate, "velocityX"), getVelocityX, setVelocityX);
	point_templ->SetAccessor(String::NewFromUtf8(isolate, "velocityY"), getVelocityY, setVelocityY);
	Local<Object> jsPoint = point_templ->NewInstance();
	global->Set(String::NewFromUtf8(isolate, "point"), jsPoint);

	Local<ObjectTemplate> mouse_templ = ObjectTemplate::New(isolate);
	mouse_templ->SetInternalFieldCount(1);
	mouse_templ->SetAccessor(String::NewFromUtf8(isolate, "x"), getMouseX, setMouseX);
	mouse_templ->SetAccessor(String::NewFromUtf8(isolate, "y"), getMouseY, setMouseY);
	Local<Object> jsMouse = mouse_templ->NewInstance();
	global->Set(String::NewFromUtf8(isolate, "mouse"), jsMouse);
		

	// load custom point logic in lua script
#if SCRIPT_MODE == ALL_SCRIPT
	char * scriptName = "scripts/allScript.js"; 
#elif SCRIPT_MODE == ALL_SCRIPT_MIN_CALLBACK
	char * scriptName = "scripts/allScriptMinCallback.js"; 
#else
	char * scriptName = "scripts/scriptToNative.js";
#endif

	std::ifstream in(scriptName, std::ios::in | std::ios::binary);
	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();
	Local<String> source = String::NewFromUtf8(isolate, contents.c_str());//"point.x += 1; stepPoint(point);");
	Local<Script> script = Script::Compile(source);

#if GAME_MODE != TEST
	// enter the loop
	while (!checkEndSingal()) {
#else
	startMeasurement();

	for (int i = 0; i < CYCLES; i++) {
#endif

		updateMouse();

		// update points
		for (int i = 0; i < POINT_COUNT; i++) {
#if SCRIPT_MODE == NATIVE
			updateVelocity(points + i);
			stepPoint(points + i);
#else
			jsPoint->SetInternalField(0, External::New(isolate, getPoint(i)));
			jsMouse->SetInternalField(0, External::New(isolate, getMouse()));
			script->Run();
#endif
		}

		drawPoints();
	}

	stopMeasurement();

	cleanupDrawing();

	displayMeasurement();

	isolate->Dispose();
	V8::Dispose();
	V8::ShutdownPlatform();
	delete platform;
	return 0;
}

void initScript() {
}

void loopScript(int pointIndex) {
}

void cleanupScript() {
}