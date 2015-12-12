#ifndef SB_GS_H
#define SB_GS_H

#include "../scenario/scenario.h"

#define SS_GS_SCRIPT_PATH "script"

namespace gs {

	// main scenarios
	int runExpression(int c, char** v);
	int runCallback(int c, char** v);

	static const ::CallbackMap scenarioMap[] = {
		{ "expression", runExpression },
		{ "callback", runCallback }
	};
};

#endif
