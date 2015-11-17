#include "../gs.h"
#include "../../scenarios/callback/callback.h"
#include "../../measure.h"
#include "gameScript.h"

namespace gs
{
	namespace callback
	{
		// callback function definitions
		double cb0() { return ::callback::processCallback(1); }
		double cb1(double a) { return ::callback::processCallback(a); }
		double cb2(double a, double b) { return ::callback::processCallback(a + b); }
		double cb3(double a, double b, double c) { return ::callback::processCallback(a + b + c); }
		double cb4(double a, double b, double c, double d) { return ::callback::processCallback(a + b + c + d); }

		void* fncs[] = { cb0, cb1, cb2, cb3, cb4 };
	}

	int runCallback(int c, char** v)
	{
		if (::callback::readArgs(c, v)) return 1;

		char source[45 + 20 + SB_C_PARAM_STRINGS_MAX_LENGTH]; // base string literal + long digits(64bits) + callback str max length
		sprintf(source, "declare c(%s) f() {return for(i=1,i<%d,1) c(%s);}", 
			::callback::getParamDeclarationString(), ::callback::getCycleCount()+1, ::callback::getParamCallString());

		gs::Script *src = gs::Script::parseString(source);
		gs::CompiledScript *script = src->compile();
		src->free();
		script->bindExternal("c", callback::fncs[::callback::getParamCount()]);
		double(*f)() = (double(*)())script->getFunction("f");

		::measure::cpuStart();
		f();
		::measure::cpuStop();

		::callback::validateResults();
		::measure::cpuDisplayResults();
		script->free();
		return 0;
	}
}