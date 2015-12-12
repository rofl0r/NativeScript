#include "../../scenario/pointSimul/pointSimul.h"
#include <stdio.h>
#include "lua.hpp"
#include "../lua.h"
#include "lua_pointSimul.h"
#include "Windows.h"

using namespace pointSimul;

namespace lua {
	namespace pointSimul {

		// TODO: get rid of this global
		lua_State* L;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptLoop(int pointIndex) {

			// load script function to be run
			lua_getglobal(L, "allScript");

			// push point parameter
			point** pointPtr = pushNewPoint(L);
			*(pointPtr) = getPoint(pointIndex);

			// push mouse parameter
			mouse** mousePtr = pushNewMouse(L);
			*(mousePtr) = getMouse();

			// push friction parameter
			lua_pushnumber(L, SB_PS_FRICTION);

			// call the script function (3 arguments, no result)
			if (lua_pcall(L, 3, 0, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}
		}

		/*
		Run scenario variant
		*/
		int allScript(int c, int p)
		{
			L = init();

			definePoint(L);
			defineMouse(L);
			loadScriptFromFile(L, "pointSimul.lua");

			int result = run(&allScriptLoop);

			close(L);

			return result;
		}
	}
}
