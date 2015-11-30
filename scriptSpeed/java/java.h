#ifndef SB_JAVA_H
#define SB_JAVA_H

#include "../scenarios/scenarios.h"

namespace java {

	// main scenarios
	int runHelloWorld(int c, char** v);

	static const ::CallbackMap scenarioMap[] = {
		{ "helloWorld", runHelloWorld }
	};

	// common util functions
};

#endif
