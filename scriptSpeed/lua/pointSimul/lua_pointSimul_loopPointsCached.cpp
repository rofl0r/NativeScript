#include <stdio.h>

#include "lua.hpp"

#include "../../measure.h"
#include "../../scenario/pointSimul/pointSimul.h"
#include "../lua.h"
#include "lua_pointSimul.h"
#include "lua_pointSimul_loop.h"

using namespace pointSimul;

namespace lua {
	namespace pointSimul {

		/*
		Run scenario variant
		*/
		int loopPointsCached(int c, int p)
		{
			lua_State* L = init();

			definePoint(L);
			defineMouse(L);
			registerCallbacks(L);
			loadScriptFromFile(L, "pointSimul.lua");

			measure::start();

			// load script function to be run and push parameters
			lua_getglobal(L, "loopPointsCached");
			lua_pushnumber(L, p);
			lua_pushnumber(L, c);
			lua_pushnumber(L, SS_PS_FRICTION);

			// call the script function (3 arguments, no result)
			if (lua_pcall(L, 3, 0, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}

			measure::stop();
			measure::displayResults();

			close(L);

			return 0;
		}
	}
}
