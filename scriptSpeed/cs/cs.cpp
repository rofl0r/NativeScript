#include <string.h>
#include <cstdlib>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "cs.h"
#include "../settings.h"

namespace cs
{

	void* callMethod(MonoMethod* method, void** params)
	{
		return mono_object_unbox(mono_runtime_invoke(method, NULL, params, NULL));
	}

	MonoDomain* init()
	{
		// create domain
		#ifdef SS_PLATFORM_WINDOWS
			#if !defined(SS_MONO_LIB_DIR) || !defined(SS_MONO_ETC_DIR)
				#error Macros SS_MONO_LIB_DIR and SS_MONO_ETC_DIR were not defined. Cannot compile, mono wouldnt work.
			#endif
				mono_set_dirs(SS_MONO_LIB_DIR, SS_MONO_ETC_DIR);
		#endif
		MonoDomain *domain = mono_jit_init_version("scriptSpeed", "v4.0");

		return domain;
	}

	void close(MonoDomain* domain)
	{
		mono_jit_cleanup(domain);
		// remove compiled scripts if some were created
		remove("CsScriptsTmp.dll");
	}

	MonoMethod* getMethod(MonoAssembly* assembly, const char* nmspace, const char* clazz, const char* method, int argcnt)
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

	// compiles source trough manual mono process call
	MonoAssembly* compileFile(MonoDomain* domain, const char* fileName)
	{	
		#if defined(SS_PLATFORM_WINDOWS)
			#if !defined(SS_MONO_MCS_DIR)
				#error Macro SS_MONO_MCS_DIR is not defined. Cannot compile, mono wouldnt work.
			#endif
			char *s = (char*)malloc(strlen(fileName) + strlen(SS_MONO_MCS_DIR) + strlen(SS_CS_SCRIPT_PATH) + 60);
			sprintf(s, "\"%s\\mcs.exe\" %s/%s -out:CsScriptsTmp.dll -warn:0", SS_MONO_MCS_DIR, SS_CS_SCRIPT_PATH, fileName);
		#else
		char *s = (char*)malloc(strlen(fileName) + strlen(SS_CS_SCRIPT_PATH) + 36);
			sprintf(s, "mcs %s/%s -out:CsScriptsTmp.dll -warn:0", SS_CS_SCRIPT_PATH, fileName);
		#endif
		system(s);
		free(s);

		return mono_domain_assembly_open(domain, "CsScriptsTmp.dll");
	}
	
}
