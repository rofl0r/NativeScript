#include "../lua.h"
#include "../../scenarios/callback/callback.h"
#include "../../measure.h"
#include "lua.hpp"
#include "Windows.h"
#include <iostream>

namespace lua {
	namespace callback {	

		typedef int(*luaCb)(lua_State*);

		int cb0(lua_State * L) {::callback::processCallback(1);
			return 0;}
		int cb1(lua_State * L) {::callback::processCallback(
			luaL_checknumber(L, 1)
			);return 0;}
		int cb2(lua_State * L) {::callback::processCallback(
			luaL_checknumber(L, 1)+luaL_checknumber(L, 2)
			);return 0;}
		int cb3(lua_State * L) {::callback::processCallback(
			luaL_checknumber(L, 1)+luaL_checknumber(L, 2)+luaL_checknumber(L, 3)
			);return 0;}
		int cb4(lua_State * L) {::callback::processCallback(
			luaL_checknumber(L, 1)+luaL_checknumber(L, 2)+luaL_checknumber(L, 3)+luaL_checknumber(L, 4)
			); return 0;}

		luaCb fncs[] = { cb0, cb1, cb2, cb3, cb4 };
	}

	int runCallback(int c, char** v)
	{
		if (::callback::readArgs(c, v)) return 1;

		char source[40 + 20 + SB_C_PARAM_CALL_STRING_MAX_LENGTH]; // base string literal + long digits(64bits) + callback str max length
		sprintf(source, "function f()for i=1,%d,1 do c(%s);end end",
			::callback::getCycleCount(), ::callback::getParamCallString());

		lua_State* L = init();
		luaL_dostring(L, source);

		// register callback function
		lua_pushcfunction(L, callback::fncs[::callback::getParamCount()]);
		lua_setglobal(L, "c");

		// load script function to be run and push parameters
		lua_getglobal(L, "f");;
		
		// call the script function (no argument, no result)
		measure::cpuStart();
		if (lua_pcall(L, 0, 0, 0) != 0) {
			printf("Lua Error: %s\n", lua_tostring(L, -1));
		}
		measure::cpuStop();

		// display results
		measure::cpuDisplayResults();
		::callback::validateResults();

		close(L);

		return 0;
	}
}
