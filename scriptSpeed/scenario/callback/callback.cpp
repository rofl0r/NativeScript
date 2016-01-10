#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "callback.h"

namespace callback {
	double result = 0;
	long cycleCnt = SS_C_DEFAULT_CYCLES;
	int paramCnt = SS_C_DEFAULT_PARAM_COUNT;

	bool readArgs(int c, char** v)
	{
		if (c > 0)
		{
			paramCnt = atol(v[0]);
			if (paramCnt < 0 || paramCnt > SS_C_MAX_PARAMS)
			{
				paramCnt = SS_C_DEFAULT_PARAM_COUNT;
				printf("Invalid param count, allowed only 0 to %d. Using default value.", SS_C_MAX_PARAMS);
				return 1;
			}
		}
		
		if (c > 1)
		{
			cycleCnt = atol(v[1]);
		}

		return 0;
	}

	int getParamCount()
	{
		return paramCnt;
	}

	long getCycleCount()
	{
		return cycleCnt;
	}

	bool validateResults()
	{
		
		double multipl = paramCnt;;
		multipl *= (cycleCnt+1.2)/2;
		if (multipl == 0) multipl = 1;
		if (abs(result - multipl) > 0.0001) {
			printf("Error: Callbacks result: %f, expected: %f\n", result, multipl);
			return false;
		}
		return true;
	}

	void processCallback(double val)
	{
		result += val/cycleCnt;
	}

	const char* getParamCallString()
	{
		switch (paramCnt)
		{
		case 1: return "0.1+i";
		case 2: return "0.1+i,0.1+i";
		case 3: return "0.1+i,0.1+i,0.1+i";
		case 4: return "0.1+i,0.1+i,0.1+i,0.1+i";
		default: return "";
		}
	}
}
