#include <iostream>
#include <cmath>

#include "../lua.h"

#include "../../measure.h"
#include "../../scenario/expression/expression.h"

namespace lua {

	double runNaive(lua_State* L)
	{
		const char* paramNames[] = SS_EXPRESSION_PARAM_NAMES;
		char source[8 + SS_EXPRESSION_MAX_LENGTH];
		sprintf(source, "return %s", expression::getExpression());
		luaL_loadstring(L, source);
		int expr = luaL_ref(L, LUA_REGISTRYINDEX);
		int paramCnt = expression::getParamCount();

		double r = 0;
		measure::start();
		for (long i = 0; i < SS_E_DEFAULT_CYCLES; i++) {
			for (int j = 0; j < paramCnt; j++)
			{
				lua_pushnumber(L, 0.1+i);
				lua_setglobal(L, paramNames[j]);
			}

			lua_rawgeti(L, LUA_REGISTRYINDEX, expr);
			if (lua_pcall(L, 0, 1, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}
			r += lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		measure::stop();

		return r;
	}

	double runOptimized(lua_State* L)
	{
		// wrap the expression into function
		const char* paramNames[] = SS_EXPRESSION_PARAM_NAMES;
		const int maxParamCnt = sizeof(paramNames) / sizeof(char*);
		char sourceParam[11 + 2 * maxParamCnt];
		sprintf(sourceParam, "function f(");
		int cur = 10;
		int paramCnt = expression::getParamCount();
		sourceParam[++cur] = paramNames[0][0];
		for (int j = 1; j < paramCnt; j++)
		{
			sourceParam[++cur] = ',';
			sourceParam[++cur] = paramNames[j][0];
		}
		sourceParam[++cur] = 0;

		char source[14 + sizeof(sourceParam) + SS_EXPRESSION_MAX_LENGTH];
		sprintf(source, "%s) return %s; end", sourceParam, expression::getExpression());
		luaL_dostring(L, source);
		lua_getglobal(L, "f");
		int expr = luaL_ref(L, LUA_REGISTRYINDEX);

		double r = 0;
		measure::start();
		for (double i = 0; i < SS_E_DEFAULT_CYCLES; i++) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, expr);
			for (int j = 0; j < paramCnt; j++)
			{
				lua_pushnumber(L, 0.1+i);
			}
			if (lua_pcall(L, paramCnt, 1, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}
			r += lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		measure::stop();

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
		
		measure::displayResults();
		expression::validateResult(r);

		close(L);
		return 0;
	}

}
