#include "callback.h"
#include <iostream>

namespace callback {
	double result = 0;
	long cycleCnt = SB_C_DEFAULT_CYCLES;
	int paramCnt = SB_C_DEFAULT_PARAM_COUNT;

	bool readArgs(int c, char** v)
	{
		if (c > 0)
		{
			paramCnt = atol(v[0]);
			if (paramCnt < 0 || paramCnt > 4)
			{
				paramCnt = SB_C_DEFAULT_PARAM_COUNT;
				printf("Invalid param count, allowed only 0 to 4. Using default value.");
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
		double multipl = 0;
		for (int i = 0; i < paramCnt; i++)
		{
			multipl += pow(0.7, i);
		}
		multipl *= (cycleCnt+1.0)/2;
		if (multipl == 0) multipl = 1;
		if (abs(result - multipl) > 0.0001) {
			printf("Error: Callbacks result: %f, expected: %f\n", result, multipl);
			return false;
		}
		return true;
	}

	double processCallback(double val)
	{
		result += val/cycleCnt;
		return 0;
	}

	char* getParamDeclarationString()
	{
		switch (paramCnt)
		{
		case 1: return "a";
		case 2: return "a,b";
		case 3: return "a,b,c";
		case 4: return "a,b,c,d";
		default: return "";
		}
	}

	char* getParamCallString()
	{
		switch (paramCnt)
		{
		case 1: return "i";
		case 2: return "i,i*0.7";
		case 3: return "i,i*0.7,i*0.49";
		case 4: return "i,i*0.7,i*0.49,i*0.343";
		default: return "";
		}
	}
}
