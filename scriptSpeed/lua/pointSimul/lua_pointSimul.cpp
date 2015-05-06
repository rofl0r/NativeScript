/* Lua results in ms:
						
		Release Wall (new str)
Native:        60            
ScToNa:      1700          
ScMinC:      2800          
Script:      3000         
*/

#include "../../scenarios/pointSimul/pointSimul.h"
#include "lua.hpp"
#include "lua_pointSimul.h"

using namespace pointSimul;

namespace lua {

	// entry point
	int runPointSimul(int c, char** v)
	{
		static const CallbackMap versionMap[] = {
			{ "allScript", &pointSimul::allScript },
			{ "scriptToNative", &pointSimul::scriptToNative },
			{ "allScriptMinCallback", &pointSimul::allScriptMinCallback },
			{ "allNative", &pointSimul::allNative },
			{ "loopControl", &pointSimul::loopControl },
			{ "loopPointsCached", &pointSimul::loopPointsCached },
			{ "loopPointsScripted", &pointSimul::loopPointsScripted },
			{ "loopCallback", &pointSimul::loopCallback },
			{ "loopSetGetMax", &pointSimul::loopSetGetMax },
			{ "loopSetGetMin", &pointSimul::loopSetGetMin }
		};

		return processConfig(c, v, versionMap, sizeof(versionMap) / sizeof(CallbackMap));
	}

	namespace pointSimul {	

		// Lua wrapper functions

		int setPointX(lua_State * L) {
			point ** p = static_cast<point**>(luaL_checkudata(L, 1, "PointMT"));
			(*p)->x = luaL_checknumber(L, 2);
			return 0;
		}

		int getPointX(lua_State * L) {
			point ** p = static_cast<point**>(luaL_checkudata(L, 1, "PointMT"));
			lua_pushnumber(L, (*p)->x);
			return 1;
		}

		int setPointY(lua_State * L) {
			point ** p = static_cast<point**>(luaL_checkudata(L, 1, "PointMT"));
			(*p)->y = luaL_checknumber(L, 2);
			return 0;
		}

		int getPointY(lua_State * L) {
			point ** p = static_cast<point**>(luaL_checkudata(L, 1, "PointMT"));
			lua_pushnumber(L, (*p)->y);
			return 1;
		}

		int setPointVelocityX(lua_State * L) {
			point ** p = static_cast<point**>(luaL_checkudata(L, 1, "PointMT"));
			(*p)->xVelocity = luaL_checknumber(L, 2);
			return 0;
		}

		int getPointVelocityY(lua_State * L) {
			point ** p = static_cast<point**>(luaL_checkudata(L, 1, "PointMT"));
			lua_pushnumber(L, (*p)->yVelocity);
			return 1;
		}

		int setPointVelocityY(lua_State * L) {
			point ** p = static_cast<point**>(luaL_checkudata(L, 1, "PointMT"));
			(*p)->yVelocity = luaL_checknumber(L, 2);
			return 0;
		}

		int getPointVelocityX(lua_State * L) {
			point ** p = static_cast<point**>(luaL_checkudata(L, 1, "PointMT"));
			lua_pushnumber(L, (*p)->xVelocity);
			return 1;
		}

		int getMouseX(lua_State * L) {
			mouse ** m = static_cast<mouse**>(luaL_checkudata(L, 1, "MouseMT"));
			lua_pushnumber(L, (*m)->x);
			return 1;
		}

		int getMouseY(lua_State * L) {
			mouse ** m = static_cast<mouse**>(luaL_checkudata(L, 1, "MouseMT"));
			lua_pushnumber(L, (*m)->y);
			return 1;
		}

		int stepPoint(lua_State * L) {
			point * p = *(static_cast<point**>(luaL_checkudata(L, 1, "PointMT")));
			stepPoint(p);

			return 0;
		}

		void definePoint(lua_State * L)
		{
			luaL_newmetatable(L, "PointMT");
			lua_pushvalue(L, -1);
			lua_setfield(L, -2, "__index"); // PointMT .__index = PointMT
			lua_pushcfunction(L, getPointX);
			lua_setfield(L, -2, "getX");
			lua_pushcfunction(L, setPointX);
			lua_setfield(L, -2, "setX");
			lua_pushcfunction(L, getPointY);
			lua_setfield(L, -2, "getY");
			lua_pushcfunction(L, setPointY);
			lua_setfield(L, -2, "setY");
			lua_pushcfunction(L, getPointVelocityX);
			lua_setfield(L, -2, "getVelocityX");
			lua_pushcfunction(L, setPointVelocityX);
			lua_setfield(L, -2, "setVelocityX");
			lua_pushcfunction(L, getPointVelocityY);
			lua_setfield(L, -2, "getVelocityY");
			lua_pushcfunction(L, setPointVelocityY);
			lua_setfield(L, -2, "setVelocityY");
		}

		point** pushNewPoint(lua_State * L)
		{
			point ** luaPointPointer = static_cast<point **>(lua_newuserdata(L, sizeof(point *)));
			luaL_setmetatable(L, "PointMT"); // set userdata metatable
			return luaPointPointer;
		}

		void defineMouse(lua_State * L)
		{
			luaL_newmetatable(L, "MouseMT");
			lua_pushvalue(L, -1);
			lua_setfield(L, -2, "__index"); // MouseMT .__index = MouseMT
			lua_pushcfunction(L, getMouseX);
			lua_setfield(L, -2, "getX");
			lua_pushcfunction(L, getMouseY);
			lua_setfield(L, -2, "getY");
		}

		mouse** pushNewMouse(lua_State * L)
		{
			mouse** luaMousePointer = static_cast<mouse **>(lua_newuserdata(L, sizeof(mouse *)));
			luaL_setmetatable(L, "MouseMT"); // set userdata metatable
			return luaMousePointer;
		}

		int loadScripts(lua_State * L)
		{
			// load script with functions
			if (luaL_dofile(L, "lua/scripts/pointSimul.lua") != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
				return 1;
			}

			return 0;
		}
	}
}
