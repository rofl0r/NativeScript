/*
Evaluation of 2000 of simple expressions takes ~200ms
*/

#include "../gs.h"
#include "gameScript.h"
#include "Windows.h"
#include <iostream>
#include "../../measure.h"
#include "../../scenarios/expression/expression.h"

namespace gs {

	typedef double(*gsFnc)(double, double);

	int runExpression(int c, char** v) 
	{
		//FILE* source = fopen("gs/scripts/expression.gs", "r");
		const char* source = "exp1(x y) (x-y)*(x-y) exp2(x y) x*y";
		double r = 0;
		gs::Script fncs(source);

		if (expression::isRunOptimized(c, v)) {

			gsFnc exp1 = (gsFnc)fncs.getFunction("exp1");
			gsFnc exp2 = (gsFnc)fncs.getFunction("exp2");

			measure::cpuStart();
			for (long i = 0; i < SB_E_DEFAULT_CYCLES*100; i++) {
				r += exp1(i, i*0.3);
				r += exp2(i, i*0.3);
			}
			measure::cpuStop();
		} 
		else
		{
			double params[] = {0,0};
			measure::cpuStart();
			for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
				params[0] = i;
				params[1] = i*0.3;
				r += fncs.interpretFunction("exp1", params);
				r += fncs.interpretFunction("exp2", params);
			}
			measure::cpuStop();
		}

		measure::cpuDisplayResults();
		expression::validateResult(r);

		//fclose(source);

		return 0;
	}

}
