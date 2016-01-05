#ifndef SS_LUA_H
#define SS_LUA_H

#include "lua.hpp"
#include "../scenario/scenario.h"

#define SS_LUA_SCRIPT_PATH "script"

namespace lua {

	// main scenarios
	int runExpression(int c, char** v);
	int runPointSimul(int c, char** v);
	int runCallback(int c, char** v);

	static const ::CallbackMap scenarioMap[] = {
		{ "pointSimul", runPointSimul },
		{ "expression", runExpression },
		{ "callback", runCallback }
	};

	// common util functions
	lua_State* init();
	void close(lua_State * L);
	int loadScriptFromFile(lua_State * L, char* fileName);
	void dumpFunction(lua_State * L, const char* funcName);
};

#endif
