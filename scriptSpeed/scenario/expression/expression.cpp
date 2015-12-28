#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "expression.h"

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
			case 2: return SS_EXPRESSION_PARAM_2;
			case 3: return SS_EXPRESSION_PARAM_3;
			case 4: return SS_EXPRESSION_PARAM_4;
			case 5: return SS_EXPRESSION_PARAM_5;
			case 6: return SS_EXPRESSION_PARAM_6;
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

	void validateResult(double result, double cycles)
	{
		double p[6];
		double r = 0;
		for (long i = 0; i < cycles; i++) {
			for (int j = 0; j < paramCount; j++)
			{
				p[j] = i*pow(0.7, j);
			}

			switch(paramCount)
			{
			case 2:
				r += (p[0]*p[1])+(p[0]+p[1])*(p[0]-p[1]);
				break;
			case 3:
				r += (p[0]*p[1])+(p[2]+p[1])*(p[0]-p[2]);
				break;
			case 4:
				r += (p[0]*p[1])+(p[2]+p[3])*(p[0]-p[1]);
				break;
			case 5:
				r += (p[0]*p[1])+(p[2]+p[3])*(p[4]-p[1]);
				break;
			case 6:
				r += (p[0]*p[1])+(p[2]+p[3])*(p[4]-p[5]);
				break;
			}
		}

		if (r-result > 0.00000001) // tolerance for precision errors
		{
			printf("Incorrect result. The correct result is by %f bigger\n", r-result);
		}

	}
}
