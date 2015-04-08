/* Lua results in ms:

        Release Wall   Release CPU
Native:       35          
LoopTN:      225         
Script:     2400        
*/

#include "gameLoop.h"
#include "../shared/settings.h"
#include "../shared/game.h"
#include "lua.hpp"
#include "Windows.h"

lua_State *L;
point ** luaPointPointer;
mouse ** luaMousePointer;

// entry point

int gameLoop() {
	initMeasurement();
	initScript();

	startMeasurement();
	lua_getglobal(L, "script"); // use precompiled script
	if (lua_pcall(L, 0, 0, 0) != 0) {
		std::cout << "Lua Error: " << lua_tostring(L, -1) << std::endl;
	}
	stopMeasurement();

	cleanupScript();
	displayMeasurement();

	return 0;
}

// Callbacks from script

/*
 Initializes game and drawing variables.
*/
int initGame(lua_State * L) {
	initDrawing();

	initGame();

	return 0;
}

/*
 Get cycles count
*/
int getCyclesCount(lua_State * L) {
#if GAME_MODE == TEST
	lua_pushnumber(L, CYCLES);
#else
	lua_pushnumber(L, -1); // infinite
#endif

	return 1;
}

/*
 Update all points
*/
int updatePoints(lua_State * L) {
	for (int i = 0; i < POINT_COUNT; i++) {
		point * p = getPoint(i);
		updateVelocity(p);
		stepPoint(p);
	}	

	return 0;
}

/*
 Get point count
*/
int getPointCount(lua_State * L) {
	lua_pushnumber(L, POINT_COUNT);

	return 1;
}

/*
 Get point by index
*/
int getPoint(lua_State * L) {
	*(luaPointPointer) = getPoint(lua_tonumber(L, 1));

	// TODO: return new point object, now only modifies global point
	return 0;
}

/*
 Get mouse
*/
int getMouse(lua_State * L) {
	// TODO: implement, mouse global for now
}

/*
 Step point based on its velocity
*/
int stepPoint(lua_State * L) {
	point * p = *(static_cast<point**>(luaL_checkudata(L, 1, "PointMT")));
	stepPoint(p);

	return 0;
}

/*
 Update point velocity
*/
int updateVelocity(lua_State * L) {
	point * p = *(static_cast<point**>(luaL_checkudata(L, 1, "PointMT")));
	updateVelocity(p);

	return 0;
}

/*
 Draw points
*/
int draw(lua_State * L) {
	drawPoints();

	return 0;
}

/*
 Cleans all resources.
*/
int cleanup(lua_State * L) {
	cleanupDrawing();

	return 0;
}

/*
 Updates mouse and returns 0 if loop continues or 1 if flag for app quit is raised
*/
int processInput(lua_State * L) {
	updateMouse();

	lua_pushboolean(L, checkEndSingal());
	return 1;
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

	lua_pushcfunction(L, updateVelocity);
	lua_setglobal(L, "updateVelocity");

	lua_pushcfunction(L, updatePoints);
	lua_setglobal(L, "updatePoints");

	lua_pushcfunction(L, getCyclesCount);
	lua_setglobal(L, "getCyclesCount");

	lua_pushcfunction(L, getPointCount);
	lua_setglobal(L, "getPointCount");

	lua_pushcfunction(L, getPoint);
	lua_setglobal(L, "getPoint");

	lua_pushcfunction(L, draw);
	lua_setglobal(L, "draw");

	lua_pushcfunction(L, initGame);
	lua_setglobal(L, "initGame");

	lua_pushcfunction(L, cleanup);
	lua_setglobal(L, "cleanup");

	lua_pushcfunction(L, processInput);
	lua_setglobal(L, "processInput"); 
	
	// load custom point logic in lua script
	luaL_loadfile(L, "scripts/gameControl.lua");

	lua_setglobal(L, "script"); // save script as global, so it is not deleted after first run
}

void loopScript(int pointIndex) {

}

void cleanupScript() {
	lua_close(L);
}