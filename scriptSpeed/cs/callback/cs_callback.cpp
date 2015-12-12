#include "../cs.h"
#include "../../scenario/callback/callback.h"
#include "../../measure.h"

#include <mono/jit/jit.h>
#include <atomic>

namespace cs {
	namespace callback {

		// callback function definitions
		__declspec(dllexport) void cb0(MonoException**) { ::callback::processCallback(1); }
		__declspec(dllexport) void cb1(double a, MonoException**) { ::callback::processCallback(a); }
		__declspec(dllexport) void cb2(double a, double b, MonoException**) { ::callback::processCallback(a + b); }
		__declspec(dllexport) void cb3(double a, double b, double c, MonoException**) { ::callback::processCallback(a + b + c); }
		__declspec(dllexport) void cb4(double a, double b, double c, double d, MonoException**) { ::callback::processCallback(a + b + c + d); }

		void* fncs[] = { cb0, cb1, cb2, cb3, cb4 };
	}

	int runCallback(int c, char** v)
	{
		if (::callback::readArgs(c, v)) return 1;
		
		char declVar[9*SB_C_MAX_PARAMS];
		char typeVar[7*SB_C_MAX_PARAMS];
		char* curD = declVar;
		char* curT = typeVar;
		if (::callback::getParamCount() > 0)
		{
			strcpy(curD, "double a"); 
			curD += 8;
			strcpy(curT, "double"); 
			curT += 6;
		}
		for (int i = 1; i < ::callback::getParamCount(); i++)
		{
			strcpy(curD, ",double "); 
			curD[8] = 'a' + i;
			curD += 9;
			strcpy(curT, ",double"); 
			curT += 7;
		}
		curD[0] = 0;
		curT[0] = 0;

		char source[240 + 20 + SB_C_PARAM_CALL_STRING_MAX_LENGTH + sizeof(declVar)]; // base string literal + long digits(64bits) + callback str max length
		sprintf(source, "using System;using System.Runtime.CompilerServices;class C{public static void Main(string[] args){}public static void f(){for(long i=1;i<%d;i++){c(%s);}}[MethodImplAttribute(MethodImplOptions.InternalCall)]public static extern void c(%s);}",
			::callback::getCycleCount() + 1, ::callback::getParamCallString(), declVar);

		// init
		MonoDomain* domain = init();

		// load script
		MonoAssembly *assembly = compileString(domain, source);
		if (!assembly)
		{
			printf("JIT compiled assembly failed to load");
			return 1;
		}

		sprintf(source, "C::c(%s)", typeVar);

		// register callback
		mono_add_internal_call(source, callback::fncs[::callback::getParamCount()]);

		// get main script method
		MonoMethod* csMethod = getMethod(assembly, "", "C", "f", 0);
		void(*f)(MonoException**) = (void(*)(MonoException**))mono_method_get_unmanaged_thunk(csMethod);
		
		// prepare parameters
		MonoException* ex;

		// run test
		measure::cpuStart();
		f(&ex);
		measure::cpuStop();

		// print results
		measure::cpuDisplayResults();
		::callback::validateResults();

		// cleanup
		close(domain);

		return 0;
	}
}
