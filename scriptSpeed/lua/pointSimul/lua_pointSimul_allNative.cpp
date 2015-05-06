#include "../../scenarios/pointSimul/pointSimul.h"
#include "lua_pointSimul.h"

using namespace pointSimul;

namespace lua {
	namespace pointSimul {

		/*
		The scripting that should happen for each point in every game loop
		*/
		void allNativeLoop(int pointIndex) {
			point* p = getPoint(pointIndex);
			updateVelocity(p);
			stepPoint(p);
		}

		/*
		Run scenario variant
		*/
		int allNative(int c, int p)
		{
			return run(&allNativeLoop);
		}
	}
}
