#ifndef SB_POINT_SIMUL_H
#define SB_POINT_SIMUL_H

// static settings
#define SB_PS_FRICTION 0.2
#define SB_PS_MOUSE_RADIUS 10
#define SB_PS_DEFAULT_POINT_COUNT 400
#define SB_PS_DEFAULT_CYCLES 1000

namespace pointSimul {

	struct CallbackMap {
		const char *name;
		int(*func)(int cycleCnt, int pointCnt);
	};

	struct point {
		double x;
		double y;
		double xVelocity;
		double yVelocity;
	};

	struct mouse {
		double x;
		double y;
	};

	// process settings and call apropriate callback
	int processConfig(int c, char** v, const CallbackMap* variants, int variantCount);

	// Run simulation. Takes care of everything except scripts. Scripts must be 
	// initialized before call and cleaned after call. Loop script function must be given.
	int run(void(*scriptInLoop)(int pointIndex));

	// functions useful in script in loop implementation
	mouse * getMouse();
	point * getPoint(int pointIndex);
	int getPointCount();


	////////// functions for manual control (don't use if not necessary) ///////////

	// main simulation functions
	void initSimul();

	// point
	void updateVelocity(point * p);
	void stepPoint(point* p);
	
	// input
	void simulateMouse();
	void updateMouse();
	int checkEndSingal();

	// drawing
	int initDrawing();
	void drawCurrentState();
	void cleanupDrawing();

	// parts of drawCurrentState
	void preDraw();
	void drawPoint(point* p);
	void drawPoint(double x, double y);
	void drawMouse();
	void postDraw();
}

#endif
