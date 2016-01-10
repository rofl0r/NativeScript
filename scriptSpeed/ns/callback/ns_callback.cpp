#include "nativeScript.h"

#include "../ns.h"
#include "../../scenario/callback/callback.h"
#include "../../measure.h"

namespace ns
{
	namespace callback
	{
		// callback function definitions
		double cb0() { ::callback::processCallback(1); return 0; }
		double cb1(double a) { ::callback::processCallback(a); return 0; }
		double cb2(double a, double b) { ::callback::processCallback(a + b); return 0; }
		double cb3(double a, double b, double c) { ::callback::processCallback(a + b + c); return 0; }
		double cb4(double a, double b, double c, double d) { ::callback::processCallback(a + b + c + d); return 0; }

		void* fncs[] = { (void*)cb0, (void*)cb1, (void*)cb2, (void*)cb3, (void*)cb4 };
	}

	int runCallback(int c, char** v)
	{
		if (::callback::readArgs(c, v)) return 1;

		char declVar[2 * SS_C_MAX_PARAMS];
		int curD = -1;
		if (::callback::getParamCount() > 0)
		{
			declVar[++curD] = 'a';
		}
		for (int i = 1; i < ::callback::getParamCount(); i++)
		{
			declVar[++curD] = ',';
			declVar[++curD] = 'a'+i;
		}
		declVar[++curD] = 0;

		char source[45 + 20 + SS_C_PARAM_CALL_STRING_MAX_LENGTH + sizeof(declVar)]; // base string literal + long digits(64bits) + callback str max length
		sprintf(source, "external c(%s) f() {return for(i=1,i<%ld,1) c(%s);}", 
			declVar, ::callback::getCycleCount()+1, ::callback::getParamCallString());

		Script *src = Script::parseString(source);
		CompiledScript *script = src->compile();
		src->free();
		script->bindExternal("c", callback::fncs[::callback::getParamCount()]);
		double(*f)() = (double(*)())script->getFunction("f");

		::measure::start();
		f();
		::measure::stop();

		::callback::validateResults();
		::measure::displayResults();

		script->free();
		return 0;
	}
}