// The optimized scenario is expected to run very fast, thus 100 times more cycles are executed

#include <iostream>
#include <cmath>

#include "nativeScript.h"

#include "../ns.h"
#include "../../measure.h"
#include "../../scenario/expression/expression.h"

namespace ns {

	typedef double(*nsFnc2)(double, double);
	typedef double(*nsFnc3)(double, double, double);
	typedef double(*nsFnc4)(double, double, double, double);
	typedef double(*nsFnc5)(double, double, double, double, double);
	typedef double(*nsFnc6)(double, double, double, double, double, double);

	void runNaive(Script* fncs)
	{
		char* paramNames[] = SS_EXPRESSION_PARAM_NAMES;
		double params[sizeof(paramNames) / sizeof(char*)];
		double r = 0;
		InterpretableScript* is = fncs->getInterpreter();
		int paramCnt = expression::getParamCount();
		
		measure::start();
		for (long i = 0; i < SS_E_DEFAULT_CYCLES; i++) {
			for (int j = 0; j < paramCnt; j++)
			{
				params[j] = 0.1+i;//*pow(0.7, j);
			}
			r += is->runFunction("f", params);
		}
		measure::stop();
		expression::validateResult(r);

		is->free();
	}

	void runOptimized(Script* fncs)
	{
		CompiledScript* cs = fncs->compile();
		void* fnc = cs->getFunction("f");
		nsFnc2 f2 = (nsFnc2)fnc;
		nsFnc3 f3 = (nsFnc3)fnc;
		nsFnc4 f4 = (nsFnc4)fnc;
		nsFnc5 f5 = (nsFnc5)fnc;
		nsFnc6 f6 = (nsFnc6)fnc;

		const long cycles = SS_E_DEFAULT_CYCLES * 100;
		double r = 0;
		switch (expression::getParamCount())
		{
		case 2:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				//r += f2(i, i*0.7);
				r += f2(0.1+i, 0.1+i);
			}
			measure::stop();
			break;
		case 3:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				//r += f3(i, i*0.7, i*pow(0.7, 2));
				r += f3(0.1+i, 0.1+i, 0.1+i);
			}
			measure::stop();
			break;
		case 4:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				//r += f4(i, i*0.7, i*pow(0.7, 2), i*pow(0.7, 3));
				r += f4(0.1+i, 0.1+i, 0.1+i, 0.1+i);
			}
			measure::stop();
			break;
		case 5:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				//r += f5(i, i*0.7, i*pow(0.7, 2), i*pow(0.7, 3), i*pow(0.7, 4));
				r += f5(0.1+i, 0.1+i, 0.1+i, 0.1+i, 0.1+i);
			}
			measure::stop();
			break;
		case 6:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				//r += f6(i, i*0.7, i*pow(0.7, 2), i*pow(0.7, 3), i*pow(0.7, 4), i*pow(0.7, 5));
				r += f6(0.1+i, 0.1+i, 0.1+i, 0.1+i, 0.1+i, 0.1+i);
			}
			measure::stop();
			break;
		}

		expression::validateResult(r, cycles);

		cs->free();
	}

	int runExpression(int c, char** v) 
	{
		if (!expression::readArgs(c, v))
		{
			return 1;
		}

		char* paramNames[] = SS_EXPRESSION_PARAM_NAMES;
		const int maxParamCnt = sizeof(paramNames) / sizeof(char*);
		char sourceParam[2 + 2 * maxParamCnt];
		sprintf(sourceParam, "f(");
		int cur = 1;
		sourceParam[++cur] = paramNames[0][0];
		for (int j = 1; j < expression::getParamCount(); j++)
		{
			sourceParam[++cur] = ',';
			sourceParam[++cur] = paramNames[j][0];
		}
		sourceParam[++cur] = 0;
		char source[10 + sizeof(sourceParam) + SS_EXPRESSION_MAX_LENGTH];
		sprintf(source, "%s) {return %s;}", sourceParam, expression::getExpression());
		Script* fncs = Script::parseString(source);

		expression::isRunOptimized() ? runOptimized(fncs) : runNaive(fncs);

		measure::displayResults();

		fncs->free();

		return 0;
	}

}
