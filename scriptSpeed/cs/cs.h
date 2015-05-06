#include "../scenarios/scenarios.h"
#include <mono/jit/jit.h>

namespace cs {
	int runExpression(int c, char** v);
	int runPointSimul(int c, char** v);

	MonoDomain* init();
	void close(MonoDomain* domain);
	MonoAssembly* compileSource(MonoDomain* domain, const char* fileName);
	MonoMethod* getMethod(MonoAssembly* assembly, char* nmspace, char* clazz, char* method, int argcnt);
	void* callMethod(MonoMethod* method, void** params);

	static const ::CallbackMap scenarioMap[] = {
		{ "expression", &runExpression },
		{ "pointSimul", &runPointSimul }
	};	
}