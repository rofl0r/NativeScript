#include "../js.h"
#include "include/v8.h"
#include "Windows.h"
#include <iostream>
#include "../../measure.h"
#include "../../scenarios/expression/expression.h"

#define SB_JS_EXPRESSION_1 "(x-y)*(x-y)"
#define SB_JS_EXPRESSION_2 "x * y"

using namespace v8;

namespace js {
	
	double runNaive(Isolate* is, Local<Context> context)
	{
		Local<String> source1 = String::NewFromUtf8(is, SB_JS_EXPRESSION_1);
		Local<Script> script1 = Script::Compile(source1);
		Local<String> source2 = String::NewFromUtf8(is, SB_JS_EXPRESSION_2);
		Local<Script> script2 = Script::Compile(source2);

		double r = 0;
		measure::cpuStart();
		Local<String> xName = v8::String::NewFromUtf8(is, "x");
		Local<String> yName = v8::String::NewFromUtf8(is, "y");
		for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
			Local<Number> x = Number::New(is, i);
			Local<Number> y = Number::New(is, i*0.3);

			context->Global()->Set(xName, x);
			context->Global()->Set(yName, y);
			r += script1->Run()->NumberValue();

			context->Global()->Set(xName, x);
			context->Global()->Set(yName, y);
			r += script2->Run()->NumberValue();
		}
		measure::cpuStop();

		return r;
	}

	double runOptimized(Isolate* is, Local<Context> context)
	{
		char source[55+11+5];
		sprintf(source, "function a(x, y) {return %s}; function b(x, y) {return %s}", SB_JS_EXPRESSION_1, SB_JS_EXPRESSION_2);
		Local<String> source1 = String::NewFromUtf8(is, source);
		Local<Script> script1 = Script::Compile(source1);
		script1->Run();
		Local<Function> f1 = getFunction(is, context, "a");
		Local<Function> f2 = getFunction(is, context, "b");

		double r = 0;
		measure::cpuStart();
		for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
			Handle<Value> argv[2] = { Number::New(is, i), Number::New(is, i*0.3) };
			r += f1->Call(context->Global(), 2, argv)->NumberValue();
			r += f2->Call(context->Global(), 2, argv)->NumberValue();
		}
		measure::cpuStop();

		return r;
	}

	int runExpression(int c, char** v) 
	{
		Isolate* is = init();
		{
			// Create a stack-allocated handle scope.
			HandleScope handle_scope(is);

			// Create global template
			Local<Context> context = Context::New(is);
			Context::Scope context_scope(context);

			double r = expression::isRunOptimized(c, v) ? runOptimized(is, context) : runNaive(is, context);

			measure::cpuDisplayResults();
			printf("Result: %f", r);
		}

		close(is);

		return 0;
	}

}
