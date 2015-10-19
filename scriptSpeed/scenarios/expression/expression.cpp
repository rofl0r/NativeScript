#include "expression.h"
#include <iostream>

namespace expression {

	bool isRunOptimized(int c, char** v)
	{
		if (c > 0)
		{
			if (c == 1 && strcmp(v[0], "opt") == 0) {
				return true;
			} else {
				printf("Incorrect arguments. Ignoring and running the default naive implementation.");
				return false;
			}
		}
		return false;
	}

	void validateResult(double result)
	{
		double r = 0;
		for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
			double x = i;
			double y = i*0.3;
			r += (x-y)*(x-y);
			r += x*y;
		}

		if (r != result)
		{
			printf("Incorrect result. The correct result is by %f bigger\n", r-result);
		}
	}
}
