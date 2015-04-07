#include "simple.h"
#include "lua.hpp"
#include "Windows.h"
#include <iostream>

#define cycles 10000

char* functionToExecuteLua() {
	return "x = 3; y = 4; z = x + y;";
}

int functionToExecuteNative(int x, int y) {
	return x + y;
}

static int functionToExecute(lua_State *L) {
	lua_pushnumber(L, functionToExecuteNative(
		lua_tonumber(L, 1),
		lua_tonumber(L, 2)
		));  /* push result */
	return 1;  /* number of results */
}

double testLua() {
	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	double time;

	// get ticks per second
	QueryPerformanceFrequency(&frequency);

	// create new Lua state
	lua_State *lua_state;
	lua_state = luaL_newstate();

	// load Lua libraries
	static const luaL_Reg lualibs[] =
	{
		{ "base", luaopen_base },
		{ NULL, NULL }
	};

	const luaL_Reg *lib = lualibs;
	for (; lib->func != NULL; lib++)
	{
		lib->func(lua_state);
		lua_settop(lua_state, 0);
	}

	QueryPerformanceCounter(&t1);
	for (int i = 0; i < cycles; i++) {
		luaL_dostring(lua_state, functionToExecuteLua());
	}
	QueryPerformanceCounter(&t2);
	time = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;

	// close the Lua state
	lua_close(lua_state);

	return time;
}

double testNative() {
	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);

	for (int i = 0; i < cycles; i++) {
		int x = 3;
		int y = 4;
		int z = functionToExecuteNative(x, y);
	}

	QueryPerformanceCounter(&t2);
	return (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
}

double testLuaNative() {
	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	double time;

	// get ticks per second
	QueryPerformanceFrequency(&frequency);

	// create new Lua state
	lua_State *lua_state;
	lua_state = luaL_newstate();

	// load Lua libraries
	static const luaL_Reg lualibs[] =
	{
		{ "base", luaopen_base },
		{ NULL, NULL }
	};

	const luaL_Reg *lib = lualibs;
	for (; lib->func != NULL; lib++)
	{
		lib->func(lua_state);
		lua_settop(lua_state, 0);
	}

	lua_pushcfunction(lua_state, functionToExecute);
	lua_setglobal(lua_state, "functionToExecute");

	QueryPerformanceCounter(&t1);
	for (int i = 0; i < cycles; i++) {
		luaL_dostring(lua_state, "x=3;x=4; functionToExecute(x,y)");
	}
	QueryPerformanceCounter(&t2);
	time = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;

	// close the Lua state
	lua_close(lua_state);

	return time;
}

int simple() {
	printf("Running %d cycles, please wait...\n\n", cycles);

	testNative();
	testLua();
	testLuaNative();

	printf("Running again while measuring...\n\n");

	double native = testNative();
	double lua = testLua();
	double luaNative = testLuaNative();

	printf("Execution time (ms):\n");
	printf("Native: %f\n", native);
	printf("Lua: %f\n", lua);
	printf("Lua to native: %f\n\n", luaNative);

	system("pause");

	return 0;
}
