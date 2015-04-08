#include "settings.h"
#include "game.h"

point points[POINT_COUNT];
mouse m;
float mouseAngle;
LARGE_INTEGER time1, time2, freq;
FILETIME cpuTime1, sysTime1, createTime, exitTime;
FILETIME cpuTime2, sysTime2;
SDL_Event e;
bool quit;
SDL_Renderer *ren;
SDL_Window *win;

point * getPoint(int pointIndex) {
	return points + pointIndex;
}

mouse * getMouse() {
	return &m;
}

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

int initDrawing() {
#if GAME_MODE != TEST
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	win = SDL_CreateWindow("Hello World!", 100, 100, POINT_COUNT, POINT_COUNT, SDL_WINDOW_SHOWN);
	if (win == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
#endif
	return 0;
}

void drawPoints() {
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
#endif
}

void cleanupDrawing() {
#if GAME_MODE != TEST
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
#endif
}

void initMeasurement() {
#if GAME_MODE == TEST
#if MEASURE_MODE == WALL
	QueryPerformanceFrequency(&freq);
#endif
#endif
}

void startMeasurement() {
#if GAME_MODE == TEST
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
#endif
}

void stopMeasurement() {
#if GAME_MODE == TEST
#if MEASURE_MODE == WALL
	QueryPerformanceCounter(&time2);
#else
	GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &sysTime2, &cpuTime2);
#endif
#endif
}

void displayMeasurement() {
#if GAME_MODE == TEST
#if MEASURE_MODE == WALL
	double time = (double)(time2.QuadPart - time1.QuadPart) * 1000.0 / freq.QuadPart;
	printf("Total wall-time: %f\n\n", time);
#else
	double sTime1 = (double)(sysTime1.dwLowDateTime | ((unsigned long long)sysTime1.dwHighDateTime << 32)) * 0.0001;
	double sTime2 = (double)(sysTime2.dwLowDateTime | ((unsigned long long)sysTime2.dwHighDateTime << 32)) * 0.0001;
	double uTime1 = (double)(cpuTime1.dwLowDateTime | ((unsigned long long)cpuTime1.dwHighDateTime << 32)) * 0.0001;
	double uTime2 = (double)(cpuTime2.dwLowDateTime | ((unsigned long long)cpuTime2.dwHighDateTime << 32)) * 0.0001;
	printf("Total system-time: %f\n", sTime2 - sTime1);
	printf("Total user-time: %f\n\n", uTime2 - uTime1);
#endif
	system("pause");
#endif
}

int checkEndSingal() {
#if GAME_MODE != TEST
	// process events
	while (SDL_PollEvent(&e)){
		switch (e.type) {
		case SDL_QUIT: // user closes the window
		case SDL_KEYDOWN: // user presses any key
		case SDL_MOUSEBUTTONDOWN: // user clicks the mouse
			
			return 1;
		}
	}
#endif
	return 0;
}

void updateMouse() {
#if GAME_MODE == INTERACTIVE
	SDL_GetMouseState(&m.x, &m.y);
#else
	mouseAngle += 0.25;
	m.x = sin(mouseAngle)* MOUSE_RADIUS + POINT_COUNT / 2;
	m.y = cos(mouseAngle)* MOUSE_RADIUS + POINT_COUNT / 2;
#endif
}

void initGame() {
	// initialize point positions
	for (int i = 0; i < POINT_COUNT; i++) {
		points[i].x = i;
		points[i].y = i;
	}

	// initialize mouse
	mouseAngle = 0;

	// initialize loop break condition
	quit = false;
}

// Game skeleton

int runGame() {

	initMeasurement();

	initDrawing();

	initScript();

	initGame();

	startMeasurement();

#if GAME_MODE != TEST
	while (!quit) {
#else
	for (int i = 0; i < CYCLES; i++) {
#endif
		quit = checkEndSingal();
		updateMouse();

		// update points
		for (int i = 0; i < POINT_COUNT; i++) {
#if SCRIPT_MODE == NATIVE
			updateVelocity(points + i);
			stepPoint(points + i);
#else
			loopScript(i);
#endif
		}

		drawPoints();
	}
	
	stopMeasurement();

	cleanupDrawing();

	cleanupScript();

	displayMeasurement();
	
	return 0;
}