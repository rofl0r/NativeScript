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
}
