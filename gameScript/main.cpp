#include "include/gameScript.h"
#include <iostream>

using namespace gs;

typedef double(*fnc)(double, double);

extern "C" double testExtern(double X) {
	fputc((char)X, stderr);
	return 0;
}

int main(int argc, char** argv)
{
	Script* s = Script::parseFile("source.gs");
	CompiledScript *cs = s->compile();
	InterpretableScript *is = s->getInterpreter();
	s->free();
	//Script* s = Script::parseString("test(x y) if (x<y) x else y", true);

	//fnc f = (fnc)cs->getFunction("test");
	//double res = f(120, 122);

	//double params[] = { 1.2, 1.5 };
	//double res = is.interpretFunction("test", params);

	//std::cout << res << std::endl;

	cs->free();
	is->free();

	return 0;
}