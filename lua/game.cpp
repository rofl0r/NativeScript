/* Lua results in ms:
- speed linear with script difficulty
- calls to c expansive

         Debug Wall   Debug CPU   Release Wall   Release CPU
Native:      115          125           35            31
ScToNa:     4800         4800         1150          1150
ScMinC:     7900         7700         2000          2000
Script:    10500        10500         2400          2400
*/

#include "game.h"
#include "lua.hpp"
#include "Windows.h"
#include <SDL.h>
#include <iostream>

////////////////////////////////////////// Settings /////////////////////////////////////////

// game mode
#define TEST 1 // no visualization, measurement is taking place
#define SHOW_TEST 2 // visualize test scenario
#define INTERACTIVE 3 // intractive mode to see scripted logic

#define GAME_MODE SHOW_TEST

// script mode (olny available if GAME_MODE = TEST)
#define NATIVE 1 // no scripting used
#define ALL_SCRIPT 2 // all point logic scripted
#define ALL_SCRIPT_MIN_CALLBACK 3 // all scripted, but callbacks to c++ minimized at cost of more script variables
#define SCRIPT_TO_NATIVE 4 // half scripted, half implemented in C++ and called from script

#define SCRIPT_MODE ALL_SCRIPT

// measure mode (olny available if GAME_MODE = TEST)
#define CPU 1
#define WALL 2

#define MEASURE_MODE CPU

// scenario size
#define POINT_COUNT 400
#define CYCLES 1000 // (olny available if GAME_MODE = TEST)

////////////////////////////////////////// Settings /////////////////////////////////////////

#define FRICTION 0.2
#define MOUSE_RADIUS 10

struct point {
	float x;
	float y;
	float xVelocity;
	float yVelocity;
};

struct mouse {
	int x;
	int y;
};

point points[POINT_COUNT];
mouse m;
float mouseAngle;


// Native version of computation

void updateVelocity(point * p) {
	float distance = sqrt(pow(p->x - m.x, 2) + pow(p->y - m.y, 2));
	if (distance > 20) {
		// points try to move towards mouse if far enough
		p->xVelocity += (m.x - p->x) / distance;
		p->yVelocity += (m.y - p->y) / distance;
	}
	else if (distance < 15) {
		// points try to keep some distance from mouse
		p->xVelocity -= (m.x - p->x) / distance;
		p->yVelocity -= (m.y - p->y) / distance;
	}
}

void stepPoint(point* p) {
	float speed = sqrt(pow(p->xVelocity, 2) + pow(p->yVelocity, 2));
	if (speed <= FRICTION) {
		p->xVelocity = 0;
		p->yVelocity = 0;
	}
	else {
		// apply friction
		p->xVelocity -= FRICTION*p->xVelocity / speed;
		p->yVelocity -= FRICTION*p->yVelocity / speed;

		// move points
		p->x += p->xVelocity;
		p->y += p->yVelocity;
	}
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


// Game skeleton

int game() {

#if MEASURE_MODE == WALL
	LARGE_INTEGER time1, time2, freq;
	QueryPerformanceFrequency(&freq);
#else
	FILETIME cpuTime1, sysTime1, createTime, exitTime;
	FILETIME cpuTime2, sysTime2;
#endif


#if GAME_MODE != TEST
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, POINT_COUNT, POINT_COUNT, SDL_WINDOW_SHOWN);
	if (win == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
#endif

	// initialize lua
	lua_State *L = luaL_newstate();
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
	point ** luaPointPointer = static_cast<point **>(lua_newuserdata(L, sizeof(point *)));
	luaL_setmetatable(L, "PointMT"); // set userdata metatable
	lua_setglobal(L, "point"); // specify the name of object in lua

	luaL_newmetatable(L, "MouseMT");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index"); // ListMT .__index = ListMT
	lua_pushcfunction(L, getMouseX);
	lua_setfield(L, -2, "getX");
	lua_pushcfunction(L, getMouseY);
	lua_setfield(L, -2, "getY");
	mouse ** luaMousePointer = static_cast<mouse **>(lua_newuserdata(L, sizeof(mouse *)));
	luaL_setmetatable(L, "MouseMT"); // set userdata metatable
	lua_setglobal(L, "mouse"); // specify the name of object in lua
	*(luaMousePointer) = &m;

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

	// initialize point positions
	for (int i = 0; i < POINT_COUNT; i++) {
		points[i].x = i;
		points[i].y = i;
	}

	// initialize mouse
	mouseAngle = 0;

#if GAME_MODE != TEST
	// enter the loop
	bool quit = false;
	SDL_Event e;
	while (!quit) {

		// process events
		while (SDL_PollEvent(&e)){
			switch (e.type) {
			case SDL_QUIT: // user closes the window
			case SDL_KEYDOWN: // user presses any key
			case SDL_MOUSEBUTTONDOWN: // user clicks the mouse
				quit = true;
				break;
			}
		}
#else
	#if SCRIPT_MODE == NATIVE
		printf("Mode: native\n");
	#elif SCRIPT_MODE == ALL_SCRIPT
		printf("Mode: all in script\n");
	#elif SCRIPT_MODE == ALL_SCRIPT_MIN_CALLBACK
		printf("Mode: all in script, callbacks minimized\n");
	#else
		printf("Mode: script to native\n");
	#endif
	printf("Running %d cycles...\n\n", CYCLES);

	#if MEASURE_MODE == WALL
		QueryPerformanceCounter(&time1);	
	#else
		GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &sysTime1, &cpuTime1);
	#endif

	for (int i = 0; i < CYCLES; i++) {
#endif

		// update mouse
#if GAME_MODE == INTERACTIVE
		SDL_GetMouseState(&m.x, &m.y);
#else
		mouseAngle += 0.25;
		m.x = sin(mouseAngle)* MOUSE_RADIUS + POINT_COUNT / 2;
		m.y = cos(mouseAngle)* MOUSE_RADIUS + POINT_COUNT / 2;
#endif

		// update points
		for (int i = 0; i < POINT_COUNT; i++) {
#if SCRIPT_MODE == NATIVE
			updateVelocity(points + i);
			stepPoint(points + i);
#else
			*(luaPointPointer) = &points[i]; // update point data
			lua_getglobal(L, "pointScript"); // use precompiled script
			if (lua_pcall(L, 0, 0, 0) != 0) {
				std::cout << "Lua Error: " << lua_tostring(L, -1) << std::endl;
			}
#endif
		}

#if GAME_MODE != TEST
		// render
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		// background
		SDL_RenderClear(ren);
		// points
		SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
		for (int i = 0; i < POINT_COUNT; i++) {
			SDL_RenderDrawPoint(ren, points[i].x, points[i].y);
		}
		// mouse
		SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		SDL_RenderDrawPoint(ren, m.x, m.y);

		SDL_RenderPresent(ren);

		SDL_Delay(20);

	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
#else
	}

	#if MEASURE_MODE == WALL
		QueryPerformanceCounter(&time2);	
		double time = (double)(time2.QuadPart - time1.QuadPart) * 1000.0 / freq.QuadPart;
		printf("Total wall-time: %f\n\n", time);
	#else
		GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &sysTime2, &cpuTime2);
		double sTime1 = (double)(sysTime1.dwLowDateTime | ((unsigned long long)sysTime1.dwHighDateTime << 32)) * 0.0001;
		double sTime2 = (double)(sysTime2.dwLowDateTime | ((unsigned long long)sysTime2.dwHighDateTime << 32)) * 0.0001;
		double uTime1 = (double)(cpuTime1.dwLowDateTime | ((unsigned long long)cpuTime1.dwHighDateTime << 32)) * 0.0001;
		double uTime2 = (double)(cpuTime2.dwLowDateTime | ((unsigned long long)cpuTime2.dwHighDateTime << 32)) * 0.0001;
		printf("Total system-time: %f\n", sTime2 - sTime1);
		printf("Total user-time: %f\n\n", uTime2 - uTime1);
	#endif
	system("pause");
#endif

	lua_close(L);
	return 0;
}