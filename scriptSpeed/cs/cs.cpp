#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "cs.h"
#include "../settings.h"

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
		#if defined(SS_PLATFORM_WINDOWS)
			#if !defined(SS_MONO_LIB_DIR) || !defined(SS_MONO_ETC_DIR)
				#error Macros SS_MONO_LIB_DIR and SS_MONO_ETC_DIR were not defined. Cannot compile, mono wouldn't work.
			#endif
				mono_set_dirs(SS_MONO_LIB_DIR, SS_MONO_ETC_DIR);
		#endif
		MonoDomain *domain = mono_jit_init_version("scriptSpeed", "v4.0");

		//mono_set_dirs("C:\\Program Files (x86)\\Mono\\lib", ":\\Program Files (x86)\\Mono\\etc");
		return domain;
	}

	void close(MonoDomain* domain)
	{
		mono_jit_cleanup(domain);
		// remove compiled scripts if some were created
		remove("CsScriptsTmp.dll");
	}

	MonoMethod* getMethod(MonoAssembly* assembly, char* nmspace, char* clazz, char* method, int argcnt)
	{
		MonoImage *	compilerImage = mono_assembly_get_image(assembly);
		MonoClass *	claz = mono_class_from_name(compilerImage, nmspace, clazz);
		MonoMethod* m = mono_class_get_method_from_name(claz, method, argcnt);
		return m;
	}

	// Creates temporary file with desired content and compiles it using compileFile()
	MonoAssembly* compileString(MonoDomain* domain, const char* source)
	{
		char *s = (char*)malloc(strlen(SS_CS_SCRIPT_PATH) + 15);
		sprintf(s, "%s/sourceTmp.cs", SS_CS_SCRIPT_PATH);
		FILE* f = fopen(s,"wb");
		fwrite(source, sizeof(char), strlen(source), f);
		fclose(f);

		MonoAssembly* ass = compileFile(domain, "sourceTmp.cs");
		remove(s);
		return ass;
	}

	// TODO, switch to compileFileCleaner()
	// compiles source trough manual mono process call
	MonoAssembly* compileFile(MonoDomain* domain, const char* fileName)
	{
		char *s = (char*)malloc(strlen(fileName) + strlen(SS_MONO_MCS_DIR) + strlen(SS_CS_SCRIPT_PATH) + 60);
		//sprintf(s, "mono CSCompiler.exe %s Scripts.dll", fileName);
		//printf("Compiling C# script...");
		#if defined(SS_PLATFORM_WINDOWS)
			#if !defined(SS_MONO_MCS_DIR)
				#error Macro SS_MONO_MCS_DIR is not defined. Cannot compile, mono wouldn't work.
			#endif
			sprintf(s, "\"%s\\mcs.exe\" %s/%s -out:CsScriptsTmp.dll -warn:0", SS_MONO_MCS_DIR, SS_CS_SCRIPT_PATH, fileName);
		#else
			sprintf(s, "mcs %s/%s -out:CsScriptsTmp.dll -warn:0", SS_CS_SCRIPT_PATH, fileName);
		#endif
		system(s);
		free(s);
		//printf(" Done\n");

		return mono_domain_assembly_open(domain, "CsScriptsTmp.dll");
	}

	// TODO fix
	// compiles source trough loading invoking compiler assembly
	MonoAssembly* compileFileCleaner(MonoDomain *domain, const char* fileName)
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
