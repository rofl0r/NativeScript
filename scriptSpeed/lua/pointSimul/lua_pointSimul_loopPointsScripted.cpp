/* Lua results in ms:

        Release Wall   Release CPU
Native:       35          
LoopTN:      225         
Script:     2400        
*/


#include <stdio.h>
#include "../../measure.h"
#include "../../scenarios/pointSimul/pointSimul.h"
#include "lua.hpp"
#include "../lua.h"
#include "lua_pointSimul.h"
#include "lua_pointSimul_loop.h"

using namespace pointSimul;

namespace lua {
	namespace pointSimul {

		/*
		Run scenario variant
		*/
		int loopPointsScripted(int c, int p)
		{
			lua_State* L = init();

			//definePoint(L);
			defineMouse(L);
			registerCallbacks(L);

			// load script with functions
			if (luaL_dofile(L, "lua/scripts/pointSimulLoopPointsScripted.lua") != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
				return 1;
			}

			measure::cpuStart();

			// load script function to be run and push parameters
			lua_getglobal(L, "loopPointsScripted");
			lua_pushnumber(L, p);
			lua_pushnumber(L, c);
			lua_pushnumber(L, SB_PS_FRICTION);

			// call the script function (3 arguments, no result)
			if (lua_pcall(L, 3, 0, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}

			measure::cpuStop();
			measure::cpuDisplayResults();

			close(L);

			return 0;
		}
	}
}
