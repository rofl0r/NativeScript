#ifndef SB_LUA_H
#define SB_LUA_H

#include "lua.hpp"
#include "../scenarios/scenarios.h"

namespace lua {
	int runExpression(int c, char** v);
	int runPointSimul(int c, char** v);

	lua_State* init();
	void close(lua_State * L);

	static const ::CallbackMap scenarioMap[] = {
		{ "pointSimul", &runPointSimul },
		{ "expression", &runExpression }
	};
};

#endif
