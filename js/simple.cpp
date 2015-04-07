#include "include/v8.h"
#include "include/libplatform/libplatform.h"
#include "Windows.h"
#include <iostream>
#include "simple.h"

#define cycles 1000

using namespace v8;

char* functionToExecuteJs() {
	//return "var x = 3; var y = 4; var z = x + y; function add(a,b) {return a+b;} z += add(x,y);";
	return "var x = 3; var y = 4; var z = x + y;";
}

int functionToExecuteNative(int x, int y) {
	return x + y;
}

void functionToExecute(const v8::FunctionCallbackInfo<v8::Value>& args) {
	int x = functionToExecuteNative(args[0]->Int32Value(), args[1]->Int32Value());
	args.GetReturnValue().Set(x);
}

double testJs() {
	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	double time;

	// get ticks per second
	QueryPerformanceFrequency(&frequency);

	// Create a new Isolate and make it the current one.
	Isolate* isolate = Isolate::New();
	{
		Isolate::Scope isolate_scope(isolate);

		// Create a stack-allocated handle scope.
		HandleScope handle_scope(isolate);

		// Create a new context.
		Local<Context> context = Context::New(isolate);

		// Enter the context for compiling and running the hello world script.
		Context::Scope context_scope(context);

		Local<String> source = String::NewFromUtf8(isolate, functionToExecuteJs());
		Local<Script> script = Script::Compile(source);
		QueryPerformanceCounter(&t1);
		for (int i = 0; i < cycles; i++) {
			Local<Value> result = script->Run();
		}
		QueryPerformanceCounter(&t2);
		//String::Utf8Value utf8(result);
		time = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	}

	// Dispose the isolate and tear down V8.
	isolate->Dispose();
	return time;
}

double testNative() {
	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);

	for (int i = 0; i < cycles; i++) {
		int x = 3;
		int y = 4;
		int z = functionToExecuteNative(x, y);
	}

	QueryPerformanceCounter(&t2);
	return (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
}

double testJsNative() {

	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	double time;

	// get ticks per second
	QueryPerformanceFrequency(&frequency);

	// Create a new Isolate and make it the current one.
	Isolate* isolate = Isolate::New();
	{
		Isolate::Scope isolate_scope(isolate);

		// Create a stack-allocated handle scope.
		HandleScope handle_scope(isolate);

		Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
		global->Set(String::NewFromUtf8(isolate, "functionToExecute"), FunctionTemplate::New(isolate, functionToExecute));

		// Create a new context.
		Local<Context> context = Context::New(isolate, NULL, global);

		// Enter the context for compiling and running the hello world script.
		Context::Scope context_scope(context);

		// JS back to Native execution
		Local<String> source = String::NewFromUtf8(isolate, "var x = 3; var y = 4; z = functionToExecute(x,y);");
		Local<Script> script = Script::Compile(source);
		QueryPerformanceCounter(&t1);
		for (int i = 0; i < cycles; i++) {
			//Local<String> source = String::NewFromUtf8(isolate, "var x = 3; var y = 4; var z = x + y; z += functionToExecute(x,y);");
			Local<Value> result = script->Run();

		}
		QueryPerformanceCounter(&t2);
		//String::Utf8Value utf8(result);
		time = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	}

	// Dispose the isolate and tear down V8.
	isolate->Dispose();
	return time;
}



int simple() {
	V8::InitializeICU();
	Platform* platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();

	printf("Running %d cycles, please wait...\n\n", cycles);

	testNative();
	testJs();
	testJsNative();

	printf("Running again while measuring...\n\n");

	double native = testNative();
	double js = testJs();
	double jsNative = testJsNative();

	printf("Execution time (ms):\n");
	printf("Native: %f\n", native);
	printf("Js: %f\n", js);
	printf("Js to native: %f\n\n", jsNative);

	system("pause");

	V8::Dispose();
	V8::ShutdownPlatform();
	delete platform;

	return 0;
}
