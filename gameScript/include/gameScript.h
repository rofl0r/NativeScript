#pragma once

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

	// Define type of external function for interpreter
	typedef double(*ExternalFunction)(int argCnt, const double* args);

	class Parser;
	class Executor;
	class Interpreter;

	// Compiled script which runs much faster but requires llvm.
	class CompiledScript
	{
	private:
		CompiledScript(Executor* exec) : exec(exec) {};
		~CompiledScript();
		Executor* exec;
		friend class Script;
	public:
		GS_IMPORT_EXPORT void bindExternal(const char* name, void* fnc); // can be run only before finalization
		GS_IMPORT_EXPORT void* getFunction(const char* name); // first call to getFunction finalizes the object
		GS_IMPORT_EXPORT void dumpCode();
		GS_IMPORT_EXPORT void free();
	};

	// Creates interpreted script which runs everywhere.
	class InterpretableScript
	{
	private:
		InterpretableScript(Interpreter* inter) : inter(inter){}
		~InterpretableScript();
		Interpreter* inter;
		friend class Script;
	public:
		GS_IMPORT_EXPORT void bindExternal(const char* name, ExternalFunction fnc);
		GS_IMPORT_EXPORT double runFunction(const char* name, double* params);
		GS_IMPORT_EXPORT void free();
	};

	class Script
	{
	private:
		Parser* parser;
		Script(Parser* parser) : parser(parser) {};
		~Script();
	public:
		GS_IMPORT_EXPORT static Script* parseString(const char* source);
		GS_IMPORT_EXPORT static Script* parseFile(const char* path);
		GS_IMPORT_EXPORT CompiledScript* compile();
		GS_IMPORT_EXPORT InterpretableScript* getInterpreter();
		GS_IMPORT_EXPORT void free();
	};

	
}
