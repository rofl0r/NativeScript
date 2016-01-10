#include <iostream>
#include <cmath>

#include "include/v8.h"

#include "../js.h"
#include "../../measure.h"
#include "../../scenario/expression/expression.h"

using namespace v8;

namespace js {
	
	double runNaive(Isolate* is, Local<Context> context)
	{
		Local<String> source = String::NewFromUtf8(is, expression::getExpression());
		Local<Script> script1 = Script::Compile(source);

		char* paramNames[] = SS_EXPRESSION_PARAM_NAMES;
		const int maxParamCnt = sizeof(paramNames) / sizeof(char*);
		Local<String> varName[maxParamCnt];
		int paramCnt = expression::getParamCount();

		for (int j = 0; j < paramCnt; j++)
		{
			varName[j] = v8::String::NewFromUtf8(is, paramNames[j]);
		}
		Local<Value> varValue[maxParamCnt];
		Local<Object> global = context->Global();
		double r = 0;
		measure::start();
		for (long i = 0; i < SS_E_DEFAULT_CYCLES; i++) {
			for (int j = 0; j < paramCnt; j++)
			{
				varValue[j] = Number::New(is, 0.1+i);
				global->Set(varName[j], varValue[j]);
			}

			r += script1->Run()->NumberValue();
		}
		measure::stop();

		return r;
	}

	double runOptimized(Isolate* is, Local<Context> context)
	{
		// wrap the expression into function
		char* paramNames[] = SS_EXPRESSION_PARAM_NAMES;
		const int maxParamCnt = sizeof(paramNames) / sizeof(char*);
		char sourceParam[11 + 2 * maxParamCnt];
		sprintf(sourceParam, "function f(");
		int cur = 10;
		int paramCnt = expression::getParamCount();
		sourceParam[++cur] = paramNames[0][0];
		for (int j = 1; j < paramCnt; j++)
		{
			sourceParam[++cur] = ',';
			sourceParam[++cur] = paramNames[j][0];
		}
		sourceParam[++cur] = 0;
		char source[11 + sizeof(sourceParam) + SS_EXPRESSION_MAX_LENGTH];
		sprintf(source, "%s) {return %s}", sourceParam, expression::getExpression());
		Local<String> source1 = String::NewFromUtf8(is, source);
		Local<Script> script1 = Script::Compile(source1);
		script1->Run();
		Local<Function> f1 = getFunction(is, context, "f");

		Local<Value> varValue[maxParamCnt];
		double r = 0;

		Local<Object> global = context->Global();
		measure::start();
		for (long i = 0; i < SS_E_DEFAULT_CYCLES; i++) {
			for (int j = 0; j < paramCnt; j++)
			{
				varValue[j] = Number::New(is, 0.1+i);
			}
			r += f1->Call(global, paramCnt, varValue)->NumberValue();
		}
		measure::stop();

		return r;
	}

	int runExpression(int c, char** v) 
	{
		if (!expression::readArgs(c, v))
		{
			return 1;
		}

		Isolate* is = init();
		{
			// Create a stack-allocated handle scope.
			HandleScope handle_scope(is);

			// Create global template
			Local<Context> context = Context::New(is);
			Context::Scope context_scope(context);

			double r = expression::isRunOptimized() ? runOptimized(is, context) : runNaive(is, context);

			measure::displayResults();
			expression::validateResult(r);
		}

		close(is);

		return 0;
	}

}
