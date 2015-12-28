#ifndef SS_LUA_POINT_SIMUL_LOOP_H
#define SS_LUA_POINT_SIMUL_LOOP_H

#include "lua.h"

namespace lua {
	namespace pointSimul {

		// Lua wrapper callbacks
		int updateVelocity(lua_State * L);
		int updatePoints(lua_State * L);
		int getPoint(lua_State * L);
		int getMouse(lua_State * L);
		int initSimul(lua_State * L);
		int draw(lua_State * L);
		int preDraw(lua_State * L);
		int postDraw(lua_State * L);
		int drawMouse(lua_State * L);
		int drawPoint(lua_State * L);
		int drawPointByCoords(lua_State * L);
		int cleanup(lua_State * L);
		int processInput(lua_State * L);

		// one function to register all wrapper callbacks
		void registerCallbacks(lua_State * L);
		int runScenario(char* name, int cycleCount, int pointCount);
	}
}

#endif
