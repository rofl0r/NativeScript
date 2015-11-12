/*
Evaluation of 2000 of simple expressions takes ~200ms
*/

#include "../lua.h"
#include "lua.hpp"
#include "Windows.h"
#include <iostream>
#include <cmath>
#include "../../measure.h"
#include "../../scenarios/expression/expression.h"

#define SB_LUA_EXPRESSION_1 "(x-y)*(x-y)"
#define SB_LUA_EXPRESSION_2 "x*y"

namespace lua {

	double runNaive(lua_State* L)
	{
		char* paramNames[] = SB_EXPRESSION_PARAM_NAMES;
		char source[8 + SB_EXPRESSION_MAX_LENGTH];
		sprintf(source, "return %s", expression::getExpression());
		luaL_loadstring(L, source);
		int expr = luaL_ref(L, LUA_REGISTRYINDEX);

		double r = 0;
		measure::cpuStart();
		for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
			for (int j = 0; j < expression::getParamCount(); j++)
			{
				lua_pushnumber(L, i*pow(0.7, j));
				lua_setglobal(L, paramNames[j]);
			}

			lua_rawgeti(L, LUA_REGISTRYINDEX, expr);
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
		// wrap the expression into function
		char* paramNames[] = SB_EXPRESSION_PARAM_NAMES;
		const int maxParamCnt = sizeof(paramNames) / sizeof(char*);
		char sourceParam[11 + 2 * maxParamCnt];
		sprintf(sourceParam, "function f(");
		int cur = 10;
		sourceParam[++cur] = paramNames[0][0];
		for (int j = 1; j < expression::getParamCount(); j++)
		{
			sourceParam[++cur] = ',';
			sourceParam[++cur] = paramNames[j][0];
		}
		sourceParam[++cur] = 0;

		char source[14 + sizeof(sourceParam) / sizeof(char) + SB_EXPRESSION_MAX_LENGTH];
		sprintf(source, "%s) return %s; end", sourceParam, expression::getExpression());
		luaL_dostring(L, source);
		lua_getglobal(L, "f");
		int expr = luaL_ref(L, LUA_REGISTRYINDEX);

		double r = 0;
		measure::cpuStart();
		for (double i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, expr);
			for (int j = 0; j < expression::getParamCount(); j++)
			{
				lua_pushnumber(L, i*pow(0.7, j));
			}
			if (lua_pcall(L, expression::getParamCount(), 1, 0) != 0) {
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
		if (!expression::readArgs(c, v))
		{
			return 1;
		}

		lua_State* L = init();

		double r = expression::isRunOptimized() ? runOptimized(L) : runNaive(L);
		
		measure::cpuDisplayResults();
		expression::validateResult(r);

		close(L);
		return 0;
	}

}
