// The optimized scenario is expected to run very fast, thus 100 times more cycles are executed

#include <cmath>

#include <mono/jit/jit.h>

#include "../cs.h"
#include "../../measure.h"
#include "../../scenario/expression/expression.h"

namespace cs {

	typedef double(__stdcall *csFnc2)(double, double, MonoException**);
	typedef double(__stdcall *csFnc3)(double, double, double, MonoException**);
	typedef double(__stdcall *csFnc4)(double, double, double, double, MonoException**);
	typedef double(__stdcall *csFnc5)(double, double, double, double, double, MonoException**);
	typedef double(__stdcall *csFnc6)(double, double, double, double, double, double, MonoException**);

	void runNaive(MonoMethod* f)
	{
		char* paramNames[] = SS_EXPRESSION_PARAM_NAMES;
		const int maxParamCnt = sizeof(paramNames) / sizeof(char*);

		double paramsRaw[maxParamCnt];
		void *params[maxParamCnt];
		for (int j = 0; j < expression::getParamCount(); j++)
		{
			params[j] = &(paramsRaw[j]);
		}
		double r = 0;

		measure::start();
		for (long i = 0; i < SS_E_DEFAULT_CYCLES; i++) {
			for (int j = 0; j < expression::getParamCount(); j++)
			{
				paramsRaw[j] = i*pow(0.7, j);
			}
			r += *(double*)callMethod(f, params);
		}
		measure::stop();

		expression::validateResult(r);
	}

	void runOptimized(MonoMethod* f)
	{
		MonoException* ex;

		void* fnc = mono_method_get_unmanaged_thunk(f);
		csFnc2 f2 = (csFnc2)fnc;
		csFnc3 f3 = (csFnc3)fnc;
		csFnc4 f4 = (csFnc4)fnc;
		csFnc5 f5 = (csFnc5)fnc;
		csFnc6 f6 = (csFnc6)fnc;

		const long cycles = SS_E_DEFAULT_CYCLES*100;
		double r = 0;
		switch (expression::getParamCount())
		{
		case 2:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				r += f2(i, i*0.7, &ex);
			}
			measure::stop();
			break;
		case 3:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				r += f3(i, i*0.7, i*pow(0.7, 2), &ex);
			}
			measure::stop();
			break;
		case 4:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				r += f4(i, i*0.7, i*pow(0.7, 2), i*pow(0.7, 3), &ex);
			}
			measure::stop();
			break;
		case 5:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				r += f5(i, i*0.7, i*pow(0.7, 2), i*pow(0.7, 3), i*pow(0.7, 4), &ex);
			}
			measure::stop();
			break;
		case 6:
			measure::start();
			for (long i = 0; i < cycles; i++) {
				r += f6(i, i*0.7, i*pow(0.7, 2), i*pow(0.7, 3), i*pow(0.7, 4), i*pow(0.7, 5), &ex);
			}
			measure::stop();
			break;
		}

		expression::validateResult(r, cycles);
	}

	int runExpression(int c, char** v)
	{
		if (!expression::readArgs(c, v))
		{
			return 1;
		}

		char* paramNames[] = SS_EXPRESSION_PARAM_NAMES;
		const int maxParamCnt = sizeof(paramNames) / sizeof(char*);
		char sourceParam[71 + 9 * maxParamCnt];
		sprintf(sourceParam, "class E{public static void Main(string[] args){}public static double f(double ");
		int cur = 77;
		sourceParam[++cur] = paramNames[0][0];
		for (int j = 1; j < expression::getParamCount(); j++)
		{
			sourceParam[++cur] = ',';
			sourceParam[++cur] = 'd';
			sourceParam[++cur] = 'o';
			sourceParam[++cur] = 'u';
			sourceParam[++cur] = 'b';
			sourceParam[++cur] = 'l';
			sourceParam[++cur] = 'e';
			sourceParam[++cur] = ' ';
			sourceParam[++cur] = paramNames[j][0];
		}
		sourceParam[++cur] = 0;
		char source[12 + sizeof(sourceParam) + SS_EXPRESSION_MAX_LENGTH];
		sprintf(source, "%s){return %s;}}", sourceParam, expression::getExpression());

		// init
		MonoDomain* domain = init();

		// load script
		MonoAssembly *assembly = compileString(domain, source);
		if (!assembly)
		{
			printf("JIT compiled assembly failed to load");
			return 1;
		}

		MonoMethod* f = getMethod(assembly, "", "E", "f", expression::getParamCount());

		expression::isRunOptimized() ? runOptimized(f) : runNaive(f);

		measure::displayResults();
		
		close(domain);
		
		return 0;
	}

}
