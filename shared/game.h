#include <SDL.h>
#include "Windows.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>

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

// functions to call
void initGame();
void updateMouse();
void updateVelocity(point * p);
void stepPoint(point* p);

int initDrawing();
int checkEndSingal();
void drawPoints();
void cleanupDrawing();

void initMeasurement();
void startMeasurement();
void stopMeasurement();
void displayMeasurement();

int runGame();
point * getPoint(int pointIndex);
mouse * getMouse();

// functions to implement
void initScript(); 
void loopScript(int pointIndex); // script login for given point
void cleanupScript(); // dispose all resources before program ends