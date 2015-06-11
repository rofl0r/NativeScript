
#include "../js.h"
#include "include/v8.h"
#include "Windows.h"
#include <iostream>
#include "../../measure.h"

#define cycles 1000000

#define SB_JS_EXPRESSION_1 "(x-y)*(x-y)"
#define SB_JS_EXPRESSION_2 "x * y"

using namespace v8;

namespace js {
	
	int runExpression(int c, char** v) 
	{
		Isolate* is = init();
		{
			// Create a stack-allocated handle scope.
			HandleScope handle_scope(is);

			// Create global template
			Local<Context> context = Context::New(is);
			Context::Scope context_scope(context);
			Local<String> source1 = String::NewFromUtf8(is, SB_JS_EXPRESSION_1);
			Local<Script> script1 = Script::Compile(source1);
			Local<String> source2 = String::NewFromUtf8(is, SB_JS_EXPRESSION_2);
			Local<Script> script2 = Script::Compile(source2);

			double r = 0;
			measure::cpuStart();
			Local<String> xName = v8::String::NewFromUtf8(is, "x");
			Local<String> yName = v8::String::NewFromUtf8(is, "y");
			for (long i = 0; i < cycles; i++) {
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
			printf("Result: %f\n", r);
			measure::cpuDisplayResults();
		}

		close(is);

		return 0;
	}
}
