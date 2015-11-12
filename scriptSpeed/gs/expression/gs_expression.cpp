/*

*/

#include "../gs.h"
#include "gameScript.h"
#include "Windows.h"
#include <iostream>
#include <cmath>
#include "../../measure.h"
#include "../../scenarios/expression/expression.h"

namespace gs {

	typedef double(*gsFnc2)(double, double);
	typedef double(*gsFnc3)(double, double, double);
	typedef double(*gsFnc4)(double, double, double, double);
	typedef double(*gsFnc5)(double, double, double, double, double);
	typedef double(*gsFnc6)(double, double, double, double, double, double);

	double runNaive(gs::Script* fncs)
	{
		char* paramNames[] = SB_EXPRESSION_PARAM_NAMES;
		double params[sizeof(paramNames) / sizeof(char*)];
		double r = 0;
		measure::cpuStart();
		for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
			for (int j = 0; j < expression::getParamCount(); j++)
			{
				params[j] = i*pow(0.7, j);
			}
			r += fncs->interpretFunction("f", params);
		}
		measure::cpuStop();

		return r;
	}

	double runOptimized(gs::Script* fncs)
	{
		void* fnc = fncs->getFunction("f");
		gsFnc2 f2 = (gsFnc2)fnc;
		gsFnc3 f3 = (gsFnc3)fnc;
		gsFnc4 f4 = (gsFnc4)fnc;
		gsFnc5 f5 = (gsFnc5)fnc;
		gsFnc6 f6 = (gsFnc6)fnc;

		const long cycles = SB_E_DEFAULT_CYCLES * 100;
		double r = 0;
		switch (expression::getParamCount())
		{
		case 2:
			measure::cpuStart();
			for (long i = 0; i < cycles; i++) {
				r += f2(i, i*0.7);
			}
			measure::cpuStop();
			break;
		case 3:
			measure::cpuStart();
			for (long i = 0; i < cycles; i++) {
				r += f3(i, i*0.7, i*pow(0.7, 2));
			}
			measure::cpuStop();
			break;
		case 4:
			measure::cpuStart();
			for (long i = 0; i < cycles; i++) {
				r += f4(i, i*0.7, i*pow(0.7, 2), i*pow(0.7, 3));
			}
			measure::cpuStop();
			break;
		case 5:
			measure::cpuStart();
			for (long i = 0; i < cycles; i++) {
				r += f5(i, i*0.7, i*pow(0.7, 2), i*pow(0.7, 3), i*pow(0.7, 4));
			}
			measure::cpuStop();
			break;
		case 6:
			measure::cpuStart();
			for (long i = 0; i < cycles; i++) {
				r += f6(i, i*0.7, i*pow(0.7, 2), i*pow(0.7, 3), i*pow(0.7, 4), i*pow(0.7, 5));
			}
			measure::cpuStop();
			break;
		}

		return r;
	}

	int runExpression(int c, char** v) 
	{
		if (!expression::readArgs(c, v))
		{
			return 1;
		}

		char* paramNames[] = SB_EXPRESSION_PARAM_NAMES;
		const int maxParamCnt = sizeof(paramNames) / sizeof(char*);
		char sourceParam[2 + 2 * maxParamCnt];
		sprintf(sourceParam, "f(");
		int cur = 1;
		sourceParam[++cur] = paramNames[0][0];
		for (int j = 1; j < expression::getParamCount(); j++)
		{
			sourceParam[++cur] = ' ';
			sourceParam[++cur] = paramNames[j][0];
		}
		sourceParam[++cur] = 0;
		char source[2 + sizeof(sourceParam) / sizeof(char) + SB_EXPRESSION_MAX_LENGTH];
		sprintf(source, "%s) %s", sourceParam, expression::getExpression());
		gs::Script* fncs = gs::Script::parseString(source, true);

		double r = expression::isRunOptimized() ? runOptimized(fncs) : runNaive(fncs);

		measure::cpuDisplayResults();
		expression::validateResult(r);

		fncs->free();

		return 0;
	}

}
