#include "../../scenarios/pointSimul/pointSimul.h"
#include <stdio.h>
#include "lua.hpp"
#include "../lua.h"
#include "lua_pointSimul.h"
#include "Windows.h"

using namespace pointSimul;

namespace lua {
	namespace pointSimul {

		// TODO: get rid of this global
		lua_State* Lsmc;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allScriptMinCallbackLoop(int pointIndex) {

			// load script function to be run
			lua_getglobal(Lsmc, "allScriptMinCallback");

			// push point parameter
			point** pointPtr = pushNewPoint(Lsmc);
			*(pointPtr) = getPoint(pointIndex);

			// push mouse parameter
			mouse** mousePtr = pushNewMouse(Lsmc);
			*(mousePtr) = getMouse();

			// push friction parameter
			lua_pushnumber(Lsmc, SB_PS_FRICTION);

			// call the script function (3 arguments, no result)
			if (lua_pcall(Lsmc, 3, 0, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(Lsmc, -1));
			}
		}

		/*
		Run scenario variant
		*/
		int allScriptMinCallback(int c, int p)
		{
			Lsmc = init();

			definePoint(Lsmc);
			defineMouse(Lsmc);
			loadScripts(Lsmc);

			int result = run(&allScriptMinCallbackLoop);

			close(Lsmc);

			return result;
		}
	}
}
