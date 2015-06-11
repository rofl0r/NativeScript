#ifndef SB_LUA_H
#define SB_LUA_H

#include "lua.hpp"
#include "../scenarios/scenarios.h"

namespace lua {

	// main scenarios
	int runExpression(int c, char** v);
	int runExpressionFnc(int c, char** v);
	int runPointSimul(int c, char** v);
	int runCallback(int c, char** v);

	static const ::CallbackMap scenarioMap[] = {
		{ "pointSimul", runPointSimul },
		{ "expression", runExpression },
		{ "callback", runCallback },
		{ "expressionFnc", runExpressionFnc }
	};

	// common util functions
	lua_State* init();
	void close(lua_State * L);
	int loadScriptFromFile(lua_State * L, char* fileName);
};

#endif
