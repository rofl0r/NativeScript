#include "expression.h"
#include <iostream>

namespace expression {

	int paramCount = 2; // default is 2 params
	bool runOptimized = false; 

	bool readArgs(int c, char** v)
	{
		if (c > 0)
		{
			paramCount = atol(v[0]);
			if (paramCount < 2 || paramCount > 6)
			{
				printf("Expression param count out of bounds: %d (min: 2, max: 6)", paramCount);
				return false;
			}

			if (c > 1) {
				if (strcmp(v[1], "opt") == 0) {
					runOptimized = true;
				}
				else 
				{
					printf("Incorrect arguments. Ignoring and running the default naive implementation.");
					return false;
				}
			}

			if (c > 2) {
				printf("Ignoring extra arguments\n");
			}
		}
		return true;
	}

	bool isRunOptimized()
	{
		return runOptimized;
	}

	int getParamCount()
	{
		return paramCount;
	}

	char* getExpression()
	{
		switch (paramCount)
		{
			case 2: return SB_EXPRESSION_PARAM_2;
			case 3: return SB_EXPRESSION_PARAM_3;
			case 4: return SB_EXPRESSION_PARAM_4;
			case 5: return SB_EXPRESSION_PARAM_5;
			case 6: return SB_EXPRESSION_PARAM_6;
			default: 
				printf("Requested argument count is not implemented in expression scenario.");
				return "";
		}
	};

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
		/*double r = 0;
		for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
			double x = i;
			double y = i*0.3;
			r += (x-y)*(x-y);
			r += x*y;
		}

		if (r != result)
		{
			printf("Incorrect result. The correct result is by %f bigger\n", r-result);
		}*/

		// TODO

		printf("Result: %f\n", result);
	}
}
