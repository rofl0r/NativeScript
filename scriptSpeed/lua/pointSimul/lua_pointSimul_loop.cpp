/* Drawing is disabled for this scenario, because switching it off 
during testing (while using this approach) is curretly not implemented.
This test is however only experimental and is not included in the final work. */

#include "lua.hpp"

#include "../../scenario/pointSimul/pointSimul.h"
#include "lua_pointSimul.h"
#include "lua_pointSimul_loop.h"
#include "../../measure.h"
#include "../lua.h"

using namespace pointSimul;

namespace lua {
	namespace pointSimul {

		void registerCallbacks(lua_State * L) {
			lua_pushcfunction(L, stepPoint);
			lua_setglobal(L, "stepPoint");

			lua_pushcfunction(L, updateVelocity);
			lua_setglobal(L, "updateVelocity");

			lua_pushcfunction(L, updatePoints);
			lua_setglobal(L, "updatePoints");

			lua_pushcfunction(L, getPoint);
			lua_setglobal(L, "getPoint");

			lua_pushcfunction(L, getMouse);
			lua_setglobal(L, "getMouse");

			lua_pushcfunction(L, draw);
			lua_setglobal(L, "draw");

			lua_pushcfunction(L, preDraw);
			lua_setglobal(L, "preDraw");

			lua_pushcfunction(L, postDraw);
			lua_setglobal(L, "postDraw");

			lua_pushcfunction(L, drawMouse);
			lua_setglobal(L, "drawMouse");

			lua_pushcfunction(L, drawPoint);
			lua_setglobal(L, "drawPoint");

			lua_pushcfunction(L, drawPointByCoords);
			lua_setglobal(L, "drawPointByCoords");

			lua_pushcfunction(L, initSimul);
			lua_setglobal(L, "initSimul");

			lua_pushcfunction(L, cleanup);
			lua_setglobal(L, "cleanup");

			lua_pushcfunction(L, processInput);
			lua_setglobal(L, "processInput");
		}

		int updateVelocity(lua_State * L) {
			point * p = *(static_cast<point**>(luaL_checkudata(L, 1, "PointMT")));
			::pointSimul::updateVelocity(p);

			return 0;
		}

		/*
		Update all points
		*/
		int updatePoints(lua_State * L) {
			for (int i = 0; i < getPointCount(); i++) {
				point * p = ::pointSimul::getPoint(i);
				updateVelocity(p);
				stepPoint(p);
			}

			return 0;
		}

		/*
		Get point by index
		*/
		int getPoint(lua_State * L) {
			point* p = ::pointSimul::getPoint((int)lua_tonumber(L, 1));
			point** pp = pushNewPoint(L);
			*(pp) = p;

			return 1;
		}

		/*
		Get mouse
		*/
		int getMouse(lua_State * L) {
			mouse** mp = pushNewMouse(L);
			*(mp) = ::pointSimul::getMouse();

			return 1;
		}

		/*
		Initializes game and drawing variables.
		*/
		int initSimul(lua_State * L) {
			//initDrawing();

			::pointSimul::initSimul();

			return 0;
		}

		/*
		Draw points
		*/
		int draw(lua_State * L) {
			//::pointSimul::drawCurrentState();

			return 0;
		}

		int preDraw(lua_State * L)
		{
			//::pointSimul::preDraw();
			return 0;
		}

		int postDraw(lua_State * L)
		{
			//::pointSimul::postDraw();
			return 0;
		}

		/*
		Draw single point by its coordinates
		*/
		int drawPointByCoords(lua_State * L) {
			double x = lua_tonumber(L, 1);
			double y = lua_tonumber(L, 2);
			//::pointSimul::drawPoint(x, y);

			return 0;
		}

		/*
		Draw single point
		*/
		int drawPoint(lua_State * L) {
			point * p = *(static_cast<point**>(luaL_checkudata(L, 1, "PointMT")));
			//::pointSimul::drawPoint(p);

			return 0;
		}

		int drawMouse(lua_State * L)
		{
			//::pointSimul::drawMouse();
			return 0;
		}

		/*
		Cleans all resources.
		*/
		int cleanup(lua_State * L) {
			//cleanupDrawing();

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

		int runScenario(char* name, int cycleCount, int pointCount)
		{
			lua_State* L = init();

			definePoint(L);
			defineMouse(L);
			registerCallbacks(L);
			loadScriptFromFile(L, "pointSimulLoop.lua");

			measure::start();

			// load script function to be run and push parameters
			lua_getglobal(L, name);
			lua_pushnumber(L, pointCount);
			lua_pushnumber(L, cycleCount);
			lua_pushnumber(L, SS_PS_FRICTION);

			// call the script function (3 arguments, no result)
			if (lua_pcall(L, 3, 0, 0) != 0) {
				printf("Lua Error: %s\n", lua_tostring(L, -1));
			}

			measure::stop();
			measure::displayResults();

			close(L);

			return 0;
		}

		int loopCallback(int c, int p)
		{
			return runScenario("loopCallback", c, p);
		}

		int loopSetGetMax(int c, int p)
		{
			return runScenario("loopSetGetMax", c, p);
		}

		int loopSetGetMin(int c, int p)
		{
			return runScenario("loopSetGetMin", c, p);
		}
	}
}
