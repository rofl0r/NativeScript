/*
Evaluation of 2000 of simple expressions takes ~200ms
*/

#include "../lua.h"
#include "lua.hpp"
#include "Windows.h"
#include <iostream>
#include "../../measure.h"
#include "../../scenarios/expression/expression.h"

#define SB_LUA_EXPRESSION_1 "(x-y)*(x-y)"
#define SB_LUA_EXPRESSION_2 "x*y"

namespace lua {

	double runNaive(lua_State* L)
	{
		char source[8 + 11];
		sprintf(source, "return %s", SB_LUA_EXPRESSION_1);
		luaL_loadstring(L, source);
		int exp1 = luaL_ref(L, LUA_REGISTRYINDEX);
		sprintf(source, "return %s", SB_LUA_EXPRESSION_2);
		luaL_loadstring(L, source);
		int exp2 = luaL_ref(L, LUA_REGISTRYINDEX);

		double r = 0;
		measure::cpuStart();
		for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
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

		return r;
	}

	double runOptimized(lua_State* L)
	{
		char source[60 + 11 + 3];
		sprintf(source, "function a(x, y) return %s; end function b(x, y) return %s; end", SB_LUA_EXPRESSION_1, SB_LUA_EXPRESSION_2);

		luaL_dostring(L, source);

		lua_getglobal(L, "a");
		int exp1 = luaL_ref(L, LUA_REGISTRYINDEX);
		lua_getglobal(L, "b");
		int exp2 = luaL_ref(L, LUA_REGISTRYINDEX);

		double r = 0;
		measure::cpuStart();
		for (double i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
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

		return r;
	}

	int runExpression(int c, char** v) 
	{
		lua_State* L = init();
		
		double r = expression::isRunOptimized(c, v) ? runOptimized(L) : runNaive(L);
		
		measure::cpuDisplayResults();
		printf("Result: %f", r);

		close(L);
		return 0;
	}

}
