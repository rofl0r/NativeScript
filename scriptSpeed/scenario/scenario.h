#ifndef SB_SCENARIO_H
#define SB_SCENARIO_H

#include <stdio.h>

struct CallbackMap {
	const char *name;
	int(*func)(int argc, char** argv);
};

int inline dummyCallback(int c, char** v)
{
	printf("Dummy callback called with the following parameters:\n");
	for (int i = 0; i < c; i++)
	{
		printf("%s\n", v[i]);
	}
	return 0;
}

#endif