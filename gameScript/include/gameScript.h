#pragma once
#include <iostream>
#include <map>

namespace gs {

	/// Define platform

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
	#define GS_PLATFORM_WINDOWS
#elif defined(__APPLE__)
	#define GS_PLATFORM_MAC
#else
	#define GS_PLATFORM_UNIX
#endif

	/// Define import or export

#if defined GS_PLATFORM_WINDOWS && !defined GS_REMOVE_DLL_HEADERS
#if defined( GS_BUILD_DLL )
	#define GS_IMPORT_EXPORT __declspec(dllexport)
#else
	#define GS_IMPORT_EXPORT __declspec(dllimport)
#endif
#else
	#define GS_IMPORT_EXPORT
#endif

	class CodeGen;

	class Script
	{
	private:
		CodeGen* codeGen;
	public:
		GS_IMPORT_EXPORT Script(FILE* file, bool dumpCode = false);
		GS_IMPORT_EXPORT Script(const char* source, bool dumpCode = false);
		GS_IMPORT_EXPORT void* getFunction(const char* name);
		GS_IMPORT_EXPORT double interpretFunction(const char* name, double* params);
		GS_IMPORT_EXPORT ~Script();
	};
}
