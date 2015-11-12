#include "../scenarios/scenarios.h"
#include <mono/jit/jit.h>

namespace cs {
	int runExpression(int c, char** v);
	int runCallback(int c, char** v);
	int runPointSimul(int c, char** v);

	MonoDomain* init();
	void close(MonoDomain* domain);
	MonoAssembly* compileFile(MonoDomain* domain, const char* fileName);
	MonoAssembly* compileString(MonoDomain* domain, const char* source);
	MonoMethod* getMethod(MonoAssembly* assembly, char* nmspace, char* clazz, char* method, int argcnt);
	void* callMethod(MonoMethod* method, void** params);

	static const ::CallbackMap scenarioMap[] = {
		{ "expression", runExpression },
		{ "pointSimul", runPointSimul },
		{ "callback", runCallback }
	};	
}