#include "../lua.h"
#include "../../scenarios/callback/callback.h"
#include "../../measure.h"
#include "lua.hpp"
#include "Windows.h"
#include <iostream>

namespace lua {
	namespace callback {	
		int callbackNative(lua_State * L) {
			::callback::processCallback(0);
			return 0;
		}
	}

	int runCallback(int c, char** v)
	{
		lua_State* L = init();
		loadScriptFromFile(L, "callback.lua");

		// register callback function
		lua_pushcfunction(L, callback::callbackNative);
		lua_setglobal(L, "callbackNative");

		// load script function to be run and push parameters
		lua_getglobal(L, "simpleCallback");
		::callback::readArgs(c, v);
		lua_pushnumber(L, ::callback::getCycleCount());
		
		// call the script function (1 argument, no result)
		measure::cpuStart();
		if (lua_pcall(L, 1, 0, 0) != 0) {
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
