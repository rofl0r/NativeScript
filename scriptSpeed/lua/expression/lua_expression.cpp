/*
Evaluation of 2000 of simple expressions takes ~200ms
*/

#include "../lua.h"
#include "lua.hpp"
#include "Windows.h"
#include <iostream>
#include "../../measure.h"

#define cycles 1000000

#define SB_LUA_EXPRESSION_1 "return (x-y)*(x-y)"
#define SB_LUA_EXPRESSION_2 "return x * y"

namespace lua {

	int runExpression(int c, char** v) 
	{
		lua_State* L = init();
		luaL_loadstring(L, SB_LUA_EXPRESSION_1);
		int exp1 = luaL_ref(L, LUA_REGISTRYINDEX);
		luaL_loadstring(L, SB_LUA_EXPRESSION_2);
		int exp2 = luaL_ref(L, LUA_REGISTRYINDEX);

		double r = 0;
		measure::cpuStart();
		for (long i = 0; i < cycles; i++) {
			lua_pushnumber(L, i);
			lua_setglobal(L, "x");
			lua_pushnumber(L, i*0.3);
			lua_setglobal(L, "y");
			lua_rawgeti(L, LUA_REGISTRYINDEX, exp1);
			if (lua_pcall(L, 0, 1, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}
			r += lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_pushnumber(L, i);
			lua_setglobal(L, "x");
			lua_pushnumber(L, i*0.3);
			lua_setglobal(L, "y");
			lua_rawgeti(L, LUA_REGISTRYINDEX, exp2);
			if (lua_pcall(L, 0, 1, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}
			r += lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		measure::cpuStop();
		printf("Result: %f\n", r);
		measure::cpuDisplayResults();

		close(L);
		return 0;
	}

	int runExpressionFnc(int c, char** v)
	{
		lua_State* L = init();
		loadScriptFromFile(L, "expression.lua");

		lua_getglobal(L, "exp1");
		int exp1 = luaL_ref(L, LUA_REGISTRYINDEX);
		lua_getglobal(L, "exp2");
		int exp2 = luaL_ref(L, LUA_REGISTRYINDEX);

		double r = 0;
		measure::cpuStart();
		for (double i = 0; i < cycles; i++) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, exp1);
			lua_pushnumber(L, i);
			lua_pushnumber(L, i*0.3);
			if (lua_pcall(L, 2, 1, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}
			r += lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_rawgeti(L, LUA_REGISTRYINDEX, exp2);
			lua_pushnumber(L, i);
			lua_pushnumber(L, i*0.3);
			if (lua_pcall(L, 2, 1, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}
			r += lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		measure::cpuStop();
		printf("Result: %f\n", r);
		measure::cpuDisplayResults();

		close(L);
		return 0;
	}
}
