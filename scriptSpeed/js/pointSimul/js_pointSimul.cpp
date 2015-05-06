/* JavaScript results in ms:

		Release Wall (new str)
Native:        60            
ScToNa:      1250          
ScMinC:      1600          
Script:      1950         
*/

#include "../../scenarios/pointSimul/pointSimul.h"
#include "js_pointSimul.h"
#include "include/v8.h"
#include <iostream>
#include <fstream>

using namespace v8;
using namespace pointSimul;

namespace js {

	// entry point
	int runPointSimul(int c, char** v)
	{
		static const CallbackMap versionMap[] = {
			{ "allScript", &pointSimul::allScript },
			{ "allNative", &pointSimul::allNative },
			{ "allScriptMinCallback", &pointSimul::allScriptMinCallback },
			{ "scriptToNative", &pointSimul::scriptToNative },
			{ "loopCallback", &pointSimul::loopCallback },
			{ "loopSetGetMax", &pointSimul::loopSetGetMax },
			{ "loopSetGetMin", &pointSimul::loopSetGetMin }
		};

		return processConfig(c, v, versionMap, sizeof(versionMap) / sizeof(CallbackMap));
	}

	namespace pointSimul {

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

		Local<ObjectTemplate> createPointDefinition(Isolate* isolate) {
			Local<ObjectTemplate> point_templ = ObjectTemplate::New(isolate);
			point_templ->SetInternalFieldCount(1);
			point_templ->SetAccessor(String::NewFromUtf8(isolate, "x"), getPointX, setPointX);
			point_templ->SetAccessor(String::NewFromUtf8(isolate, "y"), getPointY, setPointY);
			point_templ->SetAccessor(String::NewFromUtf8(isolate, "velocityX"), getVelocityX, setVelocityX);
			point_templ->SetAccessor(String::NewFromUtf8(isolate, "velocityY"), getVelocityY, setVelocityY);
			return point_templ;
		}

		Local<ObjectTemplate> createMouseDefinition(Isolate* isolate) {
			Local<ObjectTemplate> mouse_templ = ObjectTemplate::New(isolate);
			mouse_templ->SetInternalFieldCount(1);
			mouse_templ->SetAccessor(String::NewFromUtf8(isolate, "x"), getMouseX, setMouseX);
			mouse_templ->SetAccessor(String::NewFromUtf8(isolate, "y"), getMouseY, setMouseY);
			return mouse_templ;
		}

		Local<Script> loadScriptFromFile(Isolate* isolate, const char* fileName) {
			std::ifstream in(fileName, std::ios::in | std::ios::binary);
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			Local<String> source = String::NewFromUtf8(isolate, contents.c_str());//"point.x += 1; stepPoint(point);");
			return Script::Compile(source);
		}

		Handle<Function> getFunction(Isolate* isolate, Local<Context> context, const char* functionName) {
			Handle<Value> vUpdatePoint = context->Global()->Get(String::NewFromUtf8(isolate, functionName));
			return Handle<Function>::Cast(vUpdatePoint);
		}
	}
}
