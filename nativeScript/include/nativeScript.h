#ifndef NS_INCLUDE_H
#define NS_INCLUDE_H

namespace ns {

	/// Define platform

#if defined __WIN32__ || defined WIN32 || defined _WIN32
	#define NS_PLATFORM_WINDOWS
#elif defined __APPLE__
	#define NS_PLATFORM_MAC
#else
	#define NS_PLATFORM_UNIX
#endif

	/// Define import or export

#if defined NS_PLATFORM_WINDOWS && !defined NS_REMOVE_DLL_HEADERS
#if defined NS_BUILD_DLL 
	#define NS_IMPORT_EXPORT __declspec(dllexport)
#else
	#define NS_IMPORT_EXPORT __declspec(dllimport)
#endif
#else
	#define NS_IMPORT_EXPORT
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
		NS_IMPORT_EXPORT void bindExternal(const char* name, void* fnc); // can be run only before finalization
		NS_IMPORT_EXPORT void* getFunction(const char* name); // first call to getFunction finalizes the object
		NS_IMPORT_EXPORT void dumpIR();
		NS_IMPORT_EXPORT void free();
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
		NS_IMPORT_EXPORT void bindExternal(const char* name, ExternalFunction fnc);
		NS_IMPORT_EXPORT double runFunction(const char* name, double* params);
		NS_IMPORT_EXPORT void free();
	};

	class Script
	{
	private:
		Parser* parser;
		Script(Parser* parser) : parser(parser) {};
		~Script();
	public:
		NS_IMPORT_EXPORT static Script* parseString(const char* source);
		NS_IMPORT_EXPORT static Script* parseFile(const char* path);
		NS_IMPORT_EXPORT CompiledScript* compile();
		NS_IMPORT_EXPORT InterpretableScript* getInterpreter();
		NS_IMPORT_EXPORT void free();
	};
	
}

#endif
