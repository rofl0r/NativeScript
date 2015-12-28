#ifndef SB_NS_H
#define SB_NS_H

#include "../scenario/scenario.h"

#define SS_NS_SCRIPT_PATH "script"

namespace ns {

	// main scenarios
	int runExpression(int c, char** v);
	int runCallback(int c, char** v);

	static const ::CallbackMap scenarioMap[] = {
		{ "expression", runExpression },
		{ "callback", runCallback }
	};
};

#endif
