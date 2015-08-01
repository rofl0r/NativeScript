#include "../cs.h"
#include "../../scenarios/callback/callback.h"
#include "../../measure.h"

#include <mono/jit/jit.h>
#include <atomic>

namespace cs {
	namespace callback {
		typedef void(__stdcall *SimpleCallback)(double, MonoException**);

		// callback
		__declspec(dllexport) void countFnc() {
			::callback::executeCallbackBody();
		}
	}

	int runCallback(int c, char** v)
	{
		// init
		MonoDomain* domain = init();

		// load script
		MonoAssembly *assembly = compileSource(domain, "Callback.cs");
		if (!assembly)
		{
			printf("JIT compiled assembly failed to load");
			return 1;
		}

		// register callback
		mono_add_internal_call("Callback::countFnc()", callback::countFnc);

		// get main script method
		MonoMethod* csMethod = getMethod(assembly, "", "Callback", "simpleCallback", 1);
		callback::SimpleCallback methodThunk = (callback::SimpleCallback)mono_method_get_unmanaged_thunk(csMethod);
		
		// prepare parameters
		MonoException* ex;
		long cycl = ::callback::getCycleCount(c, v);

		// run test
		measure::cpuStart();
		methodThunk(cycl, &ex);
		measure::cpuStop();

		// print results
		measure::cpuDisplayResults();
		::callback::displayResults();

		// cleanup
		close(domain);

		return 0;
	}
}
