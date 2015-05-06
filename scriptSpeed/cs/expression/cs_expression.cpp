#include "../cs.h"
#include "../../measure.h"

#include <mono/jit/jit.h>

#define cycles 100000

namespace cs {
	int runExpression(int c, char** v)
	{
		// init
		MonoDomain* domain = init();

		// load script
		MonoAssembly *assembly = compileSource(domain, "cs/scripts/Expression.cs");
		if (!assembly)
		{
			printf("JIT compiled assembly failed to load");
			return 1;
		}
		MonoMethod* add = getMethod(assembly, "", "Expression", "differenceSquare", 2);
		MonoMethod* times = getMethod(assembly, "", "Expression", "times", 2);

		double r = 0;
		void *params[] = { &r, &r };
		measure::cpuStart();
		for (int i = 0; i < cycles; i++) {
			double x = i;
			double y = i*0.3;
			params[0] = &x;
			params[1] = &y;
			r += *(double*)callMethod(add, params);
			r += *(double*)callMethod(times, params);
		}
		measure::cpuStop();
		printf("Result: %f\n", r);
		measure::cpuDisplayResults();

		// cleanup
		close(domain);

		return 0;
	}
}
