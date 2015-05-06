/*
Evaluation of 2000 of simple expressions takes ~200ms
*/

#include "../lua.h"
#include "lua.hpp"
#include "Windows.h"
#include <iostream>
#include "../../measure.h"

#define cycles 100000

#define SB_LUA_EXPRESSION_1 "return (x-y)*(x-y)"
#define SB_LUA_EXPRESSION_2 "return x * y"

namespace lua {

	int runExpression(int c, char** v) 
	{
		lua_State* L = init();
		luaL_loadstring(L, SB_LUA_EXPRESSION_1);
		lua_setglobal(L, "exp1");
		luaL_loadstring(L, SB_LUA_EXPRESSION_2);
		lua_setglobal(L, "exp2");
		double r = 0;
		measure::cpuStart();
		for (int i = 0; i < cycles; i++) {
			lua_pushnumber(L, i);
			lua_setglobal(L, "x");
			lua_pushnumber(L, i*0.3);
			lua_setglobal(L, "y");
			lua_getglobal(L, "exp1");
			if (lua_pcall(L, 0, 1, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}
			r += lua_tonumber(L, -1);

			lua_pushnumber(L, i);
			lua_setglobal(L, "x");
			lua_pushnumber(L, i*0.3);
			lua_setglobal(L, "y");
			lua_getglobal(L, "exp2");
			if (lua_pcall(L, 0, 1, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}
			r += lua_tonumber(L, -1);
		}
		measure::cpuStop();
		printf("Result: %f\n", r);
		measure::cpuDisplayResults();

		close(L);
		return 0;
	}
}
