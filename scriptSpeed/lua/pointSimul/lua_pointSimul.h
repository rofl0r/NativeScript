#ifndef SB_LUA_POINT_SIMUL_H
#define SB_LUA_POINT_SIMUL_H

#include "lua.hpp"
#include "../../scenarios/pointSimul/pointSimul.h"

namespace lua {

	namespace pointSimul {
		int stepPoint(lua_State * L);
		void definePoint(lua_State * L);
		void defineMouse(lua_State * L);
		int loadScripts(lua_State * L);
		::pointSimul::point ** pushNewPoint(lua_State * L);
		::pointSimul::mouse ** pushNewMouse(lua_State * L);

		int allScript(int c, int p);
		int allScriptMinCallback(int c, int p);
		int allNative(int c, int p);
		int scriptToNative(int c, int p);
		int loopControl(int c, int p);
		int loopPointsCached(int c, int p);
		int loopPointsScripted(int c, int p);

		int loopCallback(int c, int p);
		int loopSetGetMax(int c, int p);
		int loopSetGetMin(int c, int p);
	}
}

#endif