#include "include/v8.h"
#include "include/libplatform/libplatform.h"

using namespace v8;

namespace js
{
	// TODO: get rif of the global
	Platform* platform;

	Isolate* init()
	{
		V8::InitializeICU();
		platform = platform::CreateDefaultPlatform();
		V8::InitializePlatform(platform);
		V8::Initialize();

		// initialize js
		// Create a new Isolate and make it the current one.
		Isolate* isolate = Isolate::New();
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
}
