/* Lua results in ms:
- speed linear with script difficulty
- calls to c expansive

         Release Wall   Release new structure
Native:        35              50
ScToNa:      1150            1850
ScMinC:      2000            3000
Script:      2400            3750
*/

#include "../shared/settings.h"
#include "gameGlobal.h"
#include "../shared/game.h"
#include "lua.hpp"
#include "Windows.h"

lua_State *L;
point ** luaPointPointer;
mouse ** luaMousePointer;

// entry point

int gameGlobal() {
	// give control to the shared game logic
	return runGame();
}

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


void initScript() {

	// initialize lua
	L = luaL_newstate();
	luaL_openlibs(L);

	luaL_newmetatable(L, "PointMT");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index"); // ListMT .__index = ListMT
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
	luaPointPointer = static_cast<point **>(lua_newuserdata(L, sizeof(point *)));
	luaL_setmetatable(L, "PointMT"); // set userdata metatable
	lua_setglobal(L, "point"); // specify the name of object in lua

	luaL_newmetatable(L, "MouseMT");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index"); // ListMT .__index = ListMT
	lua_pushcfunction(L, getMouseX);
	lua_setfield(L, -2, "getX");
	lua_pushcfunction(L, getMouseY);
	lua_setfield(L, -2, "getY");
	luaMousePointer = static_cast<mouse **>(lua_newuserdata(L, sizeof(mouse *)));
	luaL_setmetatable(L, "MouseMT"); // set userdata metatable
	lua_setglobal(L, "mouse"); // specify the name of object in lua
	*(luaMousePointer) = getMouse();

	lua_pushcfunction(L, stepPoint);
	lua_setglobal(L, "stepPoint");

	// load custom point logic in lua script
#if SCRIPT_MODE == ALL_SCRIPT
	luaL_loadfile(L, "scripts/allScript.lua");
#elif SCRIPT_MODE == ALL_SCRIPT_MIN_CALLBACK
	luaL_loadfile(L, "scripts/allScriptMinCallback.lua");
#else
	luaL_loadfile(L, "scripts/scriptToNative.lua");
#endif
	lua_setglobal(L, "pointScript"); // save script so global, so it is not deleted after first run

}

void loopScript(int pointIndex) {
	*(luaPointPointer) = getPoint(pointIndex); // update point data
	lua_getglobal(L, "pointScript"); // use precompiled script
	if (lua_pcall(L, 0, 0, 0) != 0) {
		std::cout << "Lua Error: " << lua_tostring(L, -1) << std::endl;
	}
}

void cleanupScript() {
	lua_close(L);
}
