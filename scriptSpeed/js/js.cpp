#include <string.h>
#include <fstream>

#include "include/v8.h"
#include "include/libplatform/libplatform.h"

#include "js.h"

using namespace v8;

namespace js
{
	Platform* platform;

	Isolate* init()
	{
		V8::InitializeICU();
		platform = platform::CreateDefaultPlatform();
		V8::InitializePlatform(platform);
		V8::Initialize();

		// initialize js
		// Create a new Isolate and make it the current one.
		Isolate::CreateParams create_params;
  		Isolate* isolate = Isolate::New(create_params);
		Isolate::Scope isolate_scope(isolate);
		return isolate;
	}

	void close(Isolate* isolate)
	{
		isolate->Dispose();
		V8::Dispose();
		V8::ShutdownPlatform();
		delete platform;
	}

	Local<Script> loadScriptFromFile(Isolate* isolate, const char* fileName) {
		char *s = (char*)malloc(strlen(fileName) + 15);
		sprintf(s, "%s/%s", SS_JS_SCRIPT_PATH, fileName);
		std::ifstream in(s, std::ios::in | std::ios::binary);
		free(s);
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		Local<String> source = String::NewFromUtf8(isolate, contents.c_str());
		return Script::Compile(source);
	}

	Handle<Function> getFunction(Isolate* isolate, Local<Context> context, const char* functionName) {
		Handle<Value> vUpdatePoint = context->Global()->Get(String::NewFromUtf8(isolate, functionName));
		return Handle<Function>::Cast(vUpdatePoint);
	}
}
