#include <atomic>
#include <string.h>

#include <mono/jit/jit.h>

#include "../cs.h"
#include "../../scenario/callback/callback.h"
#include "../../measure.h"

namespace cs {
	namespace callback {

		// callback function definitions
		SS_CS_CALLBACK_EXPORT void cb0(MonoException**) { ::callback::processCallback(1); }
		SS_CS_CALLBACK_EXPORT void cb1(double a, MonoException**) { ::callback::processCallback(a); }
		SS_CS_CALLBACK_EXPORT void cb2(double a, double b, MonoException**) { ::callback::processCallback(a + b); }
		SS_CS_CALLBACK_EXPORT void cb3(double a, double b, double c, MonoException**) { ::callback::processCallback(a + b + c); }
		SS_CS_CALLBACK_EXPORT void cb4(double a, double b, double c, double d, MonoException**) { ::callback::processCallback(a + b + c + d); }

		void* fncs[] = { (void*)cb0, (void*)cb1, (void*)cb2, (void*)cb3, (void*)cb4 };
	}

	int runCallback(int c, char** v)
	{
		if (::callback::readArgs(c, v)) return 1;
		
		char declVar[9*SS_C_MAX_PARAMS];
		char typeVar[7*SS_C_MAX_PARAMS];
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

		char source[240 + 20 + SS_C_PARAM_CALL_STRING_MAX_LENGTH + sizeof(declVar)]; // base string literal + long digits(64bits) + callback str max length
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
		measure::start();
		f(&ex);
		measure::stop();

		// print results
		measure::displayResults();
		::callback::validateResults();

		// cleanup
		close(domain);

		return 0;
	}
}
