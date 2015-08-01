#include "callback.h"
#include <iostream>

namespace callback {
	long cnt = 0;

	long getCycleCount(int c, char** v)
	{
		if (c > 0)
		{
			return atol(v[0]);
		}
		return SB_C_DEFAULT_CYCLES;
	}

	void displayResults()
	{
		printf("Callback count: %d", cnt);
	}

	void executeCallbackBody()
	{
		++cnt;
	}
}
