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
		lua_State* Lstn;

		/*
		The scripting that should happen for each point in every game loop
		*/
		void scriptToNativeLoop(int pointIndex) {

			// load script function to be run
			lua_getglobal(Lstn, "scriptToNative");

			// push point parameter
			point** pointPtr = pushNewPoint(Lstn);
			*(pointPtr) = getPoint(pointIndex);

			// push mouse parameter
			mouse** mousePtr = pushNewMouse(Lstn);
			*(mousePtr) = getMouse();

			// push friction parameter
			lua_pushnumber(Lstn, SB_PS_FRICTION);

			// call the script function (3 arguments, no result)
			if (lua_pcall(Lstn, 3, 0, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(Lstn, -1));
			}
		}

		/*
		Run scenario variant
		*/
		int scriptToNative(int c, int p)
		{
			Lstn = init();

			definePoint(Lstn);
			defineMouse(Lstn);

			lua_pushcfunction(Lstn, stepPoint);
			lua_setglobal(Lstn, "stepPoint");

			loadScriptFromFile(Lstn, "pointSimul.lua");

			int result = run(&scriptToNativeLoop);

			close(Lstn);

			return result;
		}
	}
}
