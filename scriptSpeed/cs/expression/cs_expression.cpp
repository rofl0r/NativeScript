#include "../cs.h"
#include "../../measure.h"
#include "../../scenarios/expression/expression.h"

#include <mono/jit/jit.h>

namespace cs {

	typedef double(__stdcall *TwoParamFnc)(double, double, MonoException**);

	double runNaive(MonoMethod* add, MonoMethod* times)
	{
		double r = 0;
		void *params[] = { &r, &r };
		measure::cpuStart();
		for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
			double x = i;
			double y = i*0.3;
			params[0] = &x;
			params[1] = &y;
			r += *(double*)callMethod(add, params);
			r += *(double*)callMethod(times, params);
		}
		measure::cpuStop();
		return r;
	}

	double runOptimized(MonoMethod* add, MonoMethod* times)
	{
		TwoParamFnc addThunk = (TwoParamFnc)mono_method_get_unmanaged_thunk(add);
		TwoParamFnc timesThunk = (TwoParamFnc)mono_method_get_unmanaged_thunk(times);
		MonoException* ex;

		double r = 0;
		measure::cpuStart();
		for (long i = 0; i < SB_E_DEFAULT_CYCLES; i++) {
			r += addThunk(i, i*0.3, &ex);
			r += timesThunk(i, i*0.3, &ex);
		}
		measure::cpuStop();
		return r;
	}

	int runExpression(int c, char** v)
	{
		// init
		MonoDomain* domain = init();

		// load script
		MonoAssembly *assembly = compileSource(domain, "Expression.cs");
		if (!assembly)
		{
			printf("JIT compiled assembly failed to load");
			return 1;
		}
		MonoMethod* add = getMethod(assembly, "", "Expression", "differenceSquare", 2);
		MonoMethod* times = getMethod(assembly, "", "Expression", "times", 2);

		double r = expression::isRunOptimized(c, v) ? runOptimized(add, times) : runNaive(add, times);
		
		measure::cpuDisplayResults();
		printf("Result: %f", r);
		// cleanup
		close(domain);
		
		return 0;
	}

}
