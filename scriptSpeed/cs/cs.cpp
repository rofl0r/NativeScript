#include "cs.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <windows.h>

namespace cs
{
	// TODO we need thunks for speed! but they don't seem to work...
	void* callMethod(MonoMethod* method, void** params)
	{
		// method 1 (faster, but causes access violation on program end)
		//typedef int(*methodDef) (int a, int b);
		//methodDef method = (methodDef)mono_method_get_unmanaged_thunk(methodMono);
		//printf("result: %d", method(1, 2));

		return mono_object_unbox(mono_runtime_invoke(method, NULL, params, NULL));
	}

	MonoDomain* init()
	{
		// create domain
		mono_set_dirs("..\\lib", "..\\lib");
		MonoDomain *domain = mono_jit_init_version("scriptSpeed", "v4.0.30319");

		//mono_set_dirs("C:\\Program Files (x86)\\Mono\\lib", ":\\Program Files (x86)\\Mono\\etc");
		return domain;
	}

	void close(MonoDomain* domain)
	{
		// TODO: terminate with cleanup (now it throws exception)
		//mono_jit_cleanup(domain);
	}

	MonoMethod* getMethod(MonoAssembly* assembly, char* nmspace, char* clazz, char* method, int argcnt)
	{
		MonoImage *	compilerImage = mono_assembly_get_image(assembly);
		MonoClass *	claz = mono_class_from_name(compilerImage, nmspace, clazz);
		MonoMethod* m = mono_class_get_method_from_name(claz, method, argcnt);
		return m;
	}

	// TODO, switch to compileSourceCleaner()
	// compiles source trough manual mono process call
	MonoAssembly* compileSource(MonoDomain* domain, const char* fileName)
	{
		char *s = (char*)malloc(strlen(fileName) + 60);
		//sprintf(s, "mono CSCompiler.exe %s Scripts.dll", fileName);
		//printf("Compiling C# script...");
		sprintf(s, "..\\lib\\mono\\mcs.exe cs/scripts/%s -out:CSCompiledScripts.dll -warn:0", fileName);
		system(s);
		free(s);
		//printf(" Done\n");
		return mono_domain_assembly_open(domain, "CSCompiledScripts.dll");
	}

	// TODO fix
	// compiles source trough loading invoking compiler assembly
	MonoAssembly* compileSourceCleaner(MonoDomain *domain, const char* fileName)
	{
		char * assemblyName = "CSCompiler.exe";
		MonoAssembly *compilerAssembly = mono_domain_assembly_open(domain, assemblyName);
		if (!compilerAssembly)
		{
			return NULL;
		}
		printf("x 1");
		if (mono_jit_exec(domain, compilerAssembly, 1, &assemblyName))
		{
			return NULL;
		}
		printf("x 2");
		MonoImage *	compilerImage = mono_assembly_get_image(compilerAssembly);
		printf("x 3");
		MonoClass *	assemblyBuilder = mono_class_from_name(compilerImage, "CSCompiler", "AssemblyBuilder");
		printf("x 4");
		MonoMethod * compileMethod = mono_class_get_method_from_name(assemblyBuilder, "CompileDLL", 2);
		printf("x 5");

		// thunks method
		/*typedef int(*compileMethodDef) (MonoObject* instance, MonoString* sourceFile, MonoString* outputFile);
		printf("hovno 6");
		compileMethodDef compileCS = (compileMethodDef)mono_method_get_unmanaged_thunk(compileMethod);
		printf("hovno 7");
		int errCnt = compileCS(NULL, mono_string_new(domain, fileName), mono_string_new(domain, "Script.dll"));*/
		
		// direct (slower) method
		void *params[] = { mono_string_new(domain, fileName), mono_string_new(domain, "Script.dll") };
		int errCnt = *(int*)mono_object_unbox(mono_runtime_invoke(compileMethod, NULL, params, NULL));
		
		
		if (errCnt == 0)
		{
			printf("x 8");
			return mono_domain_assembly_open(domain, "Script.dll");
		} 
		printf("x 9, errCnt = %d", errCnt);

		return NULL;
	}
	
}