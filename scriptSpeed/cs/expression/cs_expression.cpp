#include "../cs.h"
#include "../../measure.h"

#include <mono/jit/jit.h>

#define cycles 1000000

namespace cs {

	typedef double(__stdcall *TwoParamFnc)(double, double, MonoException**);

	int runExpression(int c, char** v)
	{
		// init
		MonoDomain* domain = init();
		int q = 0;

		// load script
		MonoAssembly *assembly = compileSource(domain, "Expression.cs");
		if (!assembly)
		{
			printf("JIT compiled assembly failed to load");
			return 1;
		}
		MonoMethod* add = getMethod(assembly, "", "Expression", "differenceSquare", 2);
		MonoMethod* times = getMethod(assembly, "", "Expression", "times", 2);

		TwoParamFnc addThunk = (TwoParamFnc)mono_method_get_unmanaged_thunk(add);
		TwoParamFnc timesThunk = (TwoParamFnc)mono_method_get_unmanaged_thunk(times);
		MonoException* ex;

		double r = 0;
		void *params[] = { &r, &r };
		measure::cpuStart();
		for (long i = 0; i < cycles; i++) {
			//double x = i;
			//double y = i*0.3;
			//params[0] = &x;
			//params[1] = &y;
			//r += *(double*)callMethod(add, params);
			//r += *(double*)callMethod(times, params);
			r += addThunk(i, i*0.3, &ex);
			r += timesThunk(i, i*0.3, &ex);
		}
		measure::cpuStop();
		printf("Result: %f\n", r);
		measure::cpuDisplayResults();

		// cleanup
		close(domain);

		return 0;
	}
}
