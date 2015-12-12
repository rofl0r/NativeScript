#include "pointSimul.h"
#include "../../measure.h"
#include <SDL.h>
#include "Windows.h"
#include <iostream>

namespace pointSimul {

	// TODO: get rid of globals

	int pointCount = SB_PS_DEFAULT_POINT_COUNT;
	int cycles = SB_PS_DEFAULT_CYCLES;
	point* points;
	mouse m;
	float mouseAngle;
	SDL_Event e;
	bool quit;
	SDL_Renderer *ren;
	SDL_Window *win;
	bool measureWall = false;

	point * getPoint(int pointIndex) {
		return points + pointIndex;
	}

	int getPointCount()
	{
		return pointCount;
	}

	mouse * getMouse() {
		return &m;
	}

	// Native version of computation

	void updateVelocity(point * p) {
		double distance = sqrt(pow(p->x - m.x, 2) + pow(p->y - m.y, 2));
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
		double speed = sqrt(pow(p->xVelocity, 2) + pow(p->yVelocity, 2));
		if (speed <= SB_PS_FRICTION) {
			p->xVelocity = 0;
			p->yVelocity = 0;
		}
		else {
			// apply friction
			p->xVelocity -= SB_PS_FRICTION*p->xVelocity / speed;
			p->yVelocity -= SB_PS_FRICTION*p->yVelocity / speed;

			// move points
			p->x += p->xVelocity;
			p->y += p->yVelocity;
		}
	}

	// private function, just, to have the color on single space
	void setPointDrawColor()
	{
		SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
	}

	int initDrawing() {
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			return 1;
		}

		win = SDL_CreateWindow("Hello World!", 100, 100, pointCount, pointCount, SDL_WINDOW_SHOWN);
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

		setPointDrawColor();

		return 0;
	}

	void drawCurrentState() 
	{

		preDraw();
		
		for (int i = 0; i < pointCount; i++) {
			drawPoint(points + i);
		}

		drawMouse();

		postDraw();
	}

	void preDraw()
	{
		// render
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		// background
		SDL_RenderClear(ren);
		// always return to point drawing color
		setPointDrawColor();
	}

	void postDraw()
	{
		// update screen
		SDL_RenderPresent(ren);
		// wait
		SDL_Delay(20);
	}

	void drawMouse()
	{
		// draw mouse with different color
		SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		SDL_RenderDrawPoint(ren, (int)m.x, (int)m.y);
		// always return to point drawing color
		setPointDrawColor();
	}

	void drawPoint(point* p)
	{
		SDL_RenderDrawPoint(ren, (int)p->x, (int)p->y);
	}

	void drawPoint(double x, double y)
	{
		SDL_RenderDrawPoint(ren, (int)x, (int)y);
	}

	void cleanupDrawing() {
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
	}


	int checkEndSingal() {
		// process events
		while (SDL_PollEvent(&e)){
			switch (e.type) {
			case SDL_QUIT: // user closes the window
			case SDL_KEYDOWN: // user presses any key
			case SDL_MOUSEBUTTONDOWN: // user clicks the mouse

				return 1;
			}
		}

		return 0;
	}

	void simulateMouse()
	{
		mouseAngle += 0.25;
		m.x = sin(mouseAngle)* SB_PS_MOUSE_RADIUS + pointCount / 2;
		m.y = cos(mouseAngle)* SB_PS_MOUSE_RADIUS + pointCount / 2;
	}

	void updateMouse() {
		int x;
		int y;
		SDL_GetMouseState(&x, &y);
		m.x = (double)x;
		m.y = (double)y;
	}

	void initSimul() {

		// initialize point positions
		for (int i = 0; i < pointCount; i++) {
			points[i].x = i;
			points[i].y = i;
		}

		// initialize mouse
		mouseAngle = 0;

		// initialize loop break condition
		quit = false;
	}

	
	// run interactive version without measuring
	int runInteractive(void(*scriptInLoop)(int pointIndex))
	{
		initSimul();
		initDrawing();
		while (!checkEndSingal()) {
			updateMouse();

			// update points
			for (int i = 0; i < pointCount; i++) {
				scriptInLoop(i);
			}

			drawCurrentState();
		}
		cleanupDrawing();

		return 0;
	}

	// run test with visual representation and without measuring
	int runShowTest(void(*scriptInLoop)(int pointIndex))
	{
		initSimul();
		initDrawing();
		for (int i = 0; i < cycles; i++) {
			simulateMouse();

			// update points
			for (int j = 0; j < pointCount; j++) {
				scriptInLoop(j);
			}

			drawCurrentState();
		}
		cleanupDrawing();

		return 0;
	}

	// run measured test
	int runTest(void(*scriptInLoop)(int pointIndex)) 
	{
		initSimul();
		if (measureWall)
		{
			measure::wallStart();
		} 
		else
		{
			measure::cpuStart();
		}

		for (int i = 0; i < cycles; i++) {
			simulateMouse();

			// update points
			for (int j = 0; j < pointCount; j++) {
				scriptInLoop(j);
			}
		}

		if (measureWall)
		{
			measure::wallStop();
			measure::wallDisplayResults();
		}
		else
		{
			measure::cpuStop();
			measure::cpuDisplayResults();
		}

		return 0;
	}

	// current run configuration (default is runTest)
	int(*runVersion)(void(*scriptInLoop)(int pointIndex)) = runTest;

	int run(void(*scriptInLoop)(int pointIndex))
	{
		return runVersion(scriptInLoop);
	}

	int processConfig(int c, char** v, const CallbackMap* variants, int variantCount)
	{
		if (variantCount <= 0)
		{
			printf("There are no implemented configurations for game scenario in this language.\n");
			return 1;
		}

		// default variant is first one
		if (c <= 0)
		{
			printf("Running default configuration: measured test for %s.\n", variants[0].name);
			points = new point[pointCount];
			int result = variants[0].func(cycles, pointCount);
			delete[] points;
			return result;
		}

		// select custom variant
		const CallbackMap* selectedVariant = 0;
		for (int i = 0; i < variantCount; i++)
		{
			if (strcmp(variants[i].name, v[0]) == 0)
			{
				selectedVariant = &(variants[i]);
				break;
			}
		}
		if (selectedVariant == 0) {
			printf("Requested configuration doesn't exist. You can pick from:\n");
			for (int i = 0; i < variantCount; i++)
			{
				printf("%s ", variants[i].name);
			}
			printf("\n");

			return 1;
		}

		if (c > 1)
		{
			if (strcmp("show", v[1]) == 0)
			{
				printf("Running exact visualization of test scenario.\n");
				runVersion = runShowTest;
			}
			else if (strcmp("interactive", v[1]) == 0)
			{
				printf("Running interactive visualization of test scenario.\n");
				runVersion = runInteractive;
			}
			else if (strcmp("test", v[1]) == 0)
			{
				printf("Running measured test.\n");
				//runVersion = runTest; // keep default
			}
			else
			{
				printf("Ignoring unknown argument: %s\n", v[1]);
			}
		}

		// TODO: wall/cpu

		if (c > 2)
		{
			cycles = atol(v[2]);
		}

		if (c > 3)
		{
			pointCount = atol(v[3]);
		}

		if (c > 4)
		{
			printf("Too many arguments. Ignoring unknown arguments starting from: %s\n", v[4]);
		}

		points = new point[pointCount];
		int result = selectedVariant->func(cycles, pointCount);
		delete[] points;
		return result;
	}
}
