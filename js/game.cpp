/* JavaScript results in ms:
- script starting overhead, then faster than lua
- calls to c cheap
         Debug Wall   Debug CPU   Release Wall   Release CPU
Native:                                 33            31 
ScToNa:                               1500          1550
ScMinC:                               2700          2700
Script:                               2250          2200
*/

#include "include/v8.h"
#include "include/libplatform/libplatform.h"
#include <SDL.h>
#include "Windows.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include "game.h"
using namespace v8;

////////////////////////////////////////// Settings /////////////////////////////////////////

// game mode
#define TEST 1 // no visualization, measurement is taking place
#define SHOW_TEST 2 // visualize test scenario
#define INTERACTIVE 3 // intractive mode to see scripted logic

#define GAME_MODE SHOW_TEST // intractive mode to see scripted logic


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

void stepPoint(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Local<Object> self = Local<Object>::Cast(args[0]);
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	stepPoint(static_cast<point*>(wrap->Value()));
}


// Js wrapper functions
void setPointX(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<point*>(wrap->Value())->x = value->NumberValue();
}

void getPointX(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<point*>(wrap->Value())->x);
}
void setPointY(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<point*>(wrap->Value())->y = value->NumberValue();
}

void getPointY(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<point*>(wrap->Value())->y);
}
void setVelocityX(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<point*>(wrap->Value())->xVelocity = value->NumberValue();
}

void getVelocityX(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<point*>(wrap->Value())->xVelocity);
}
void setVelocityY(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<point*>(wrap->Value())->yVelocity = value->NumberValue();
}

void getVelocityY(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<point*>(wrap->Value())->yVelocity);
}
void setMouseX(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<mouse*>(wrap->Value())->x = value->Int32Value();
}
void getMouseX(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<mouse*>(wrap->Value())->x);
}
void setMouseY(Local<String> prop, Local<Value> value, const PropertyCallbackInfo<void>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	static_cast<mouse*>(wrap->Value())->y = value->Int32Value();
}
void getMouseY(Local<String> prop, const PropertyCallbackInfo<Value>& info) {
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	info.GetReturnValue().Set(static_cast<mouse*>(wrap->Value())->y);
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

	V8::InitializeICU();
	Platform* platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();

	// initialize js
	// Create a new Isolate and make it the current one.
	Isolate* isolate = Isolate::New();
	Isolate::Scope isolate_scope(isolate);

	// Create a stack-allocated handle scope.
	HandleScope handle_scope(isolate);
	
	// Create point object wrapper and global object
	Local<ObjectTemplate> global_templ = ObjectTemplate::New(isolate);
	global_templ->Set(String::NewFromUtf8(isolate, "stepPoint"), FunctionTemplate::New(isolate, stepPoint));
	Local<Context> context = Context::New(isolate, NULL, global_templ);
	Context::Scope context_scope(context);
	Local<Object> global = Local<Object>::Cast(context->Global()->GetPrototype());
	
	Local<ObjectTemplate> point_templ = ObjectTemplate::New(isolate);
	point_templ->SetInternalFieldCount(1);
	point_templ->SetAccessor(String::NewFromUtf8(isolate, "x"), getPointX, setPointX);
	point_templ->SetAccessor(String::NewFromUtf8(isolate, "y"), getPointY, setPointY);
	point_templ->SetAccessor(String::NewFromUtf8(isolate, "velocityX"), getVelocityX, setVelocityX);
	point_templ->SetAccessor(String::NewFromUtf8(isolate, "velocityY"), getVelocityY, setVelocityY);
	Local<Object> jsPoint = point_templ->NewInstance();
	global->Set(String::NewFromUtf8(isolate, "point"), jsPoint);

	Local<ObjectTemplate> mouse_templ = ObjectTemplate::New(isolate);
	mouse_templ->SetInternalFieldCount(1);
	mouse_templ->SetAccessor(String::NewFromUtf8(isolate, "x"), getMouseX, setMouseX);
	mouse_templ->SetAccessor(String::NewFromUtf8(isolate, "y"), getMouseY, setMouseY);
	Local<Object> jsMouse = mouse_templ->NewInstance();
	global->Set(String::NewFromUtf8(isolate, "mouse"), jsMouse);
	
	
		

	// load custom point logic in lua script
#if SCRIPT_MODE == ALL_SCRIPT
	char * scriptName = "scripts/allScript.js"; 
#elif SCRIPT_MODE == ALL_SCRIPT_MIN_CALLBACK
	char * scriptName = "scripts/allScriptMinCallback.js"; 
#else
	char * scriptName = "scripts/scriptToNative.js";
#endif

	std::ifstream in(scriptName, std::ios::in | std::ios::binary);
	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();
	Local<String> source = String::NewFromUtf8(isolate, contents.c_str());//"point.x += 1; stepPoint(point);");
	Local<Script> script = Script::Compile(source);

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
			jsPoint->SetInternalField(0, External::New(isolate, &points[i]));
			jsMouse->SetInternalField(0, External::New(isolate, &m));
			script->Run();
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

	isolate->Dispose();
	V8::Dispose();
	V8::ShutdownPlatform();
	delete platform;
	return 0;
}