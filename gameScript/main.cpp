#include "include/gameScript.h"
#include <iostream>

using namespace gs;

int main(int argc, char** argv)
{
	Script s("test(x) x+1+1");

	double(*f)(double) = (double(*)(double))s.getFunction("test");

	double res = f(1.2);

	std::cout << res << std::endl;

	return 0;
}