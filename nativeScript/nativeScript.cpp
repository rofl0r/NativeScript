#include "include/nativeScript.h"
#include "ast.h"
#include "parser.h"
#include "interpreter.h"
#include "executor.h"

namespace ns {

	void CompiledScript::bindExternal(const char* name, void* fnc)
	{
		exec->bindExternal(name, fnc);
	}

	void* CompiledScript::getFunction(const char* name)
	{
		return exec->getFunction(name);
	}

	void CompiledScript::dumpIR()
	{
		exec->dumpIR();
	}

	void CompiledScript::free()
	{
		delete this;
	}

	CompiledScript::~CompiledScript()
	{
		delete exec;
	}

	void InterpretableScript::bindExternal(const char* name, ExternalFunction fnc)
	{
		inter->bindExternal(name, fnc);
	}

	double InterpretableScript::runFunction(const char* name, double* params)
	{
		return inter->runFunction(name, params);
	}

	void InterpretableScript::free()
	{
		delete this;
	}

	InterpretableScript::~InterpretableScript()
	{
		delete inter;
	}

	Script* Script::parseString(const char* source)
	{
		return new Script(new Parser(source));
	}

	Script* Script::parseFile(const char* path)
	{
		FILE* f = fopen(path, "r");
		Script* s = new Script(new Parser(f));
		fclose(f);
		return s;
	}

	CompiledScript* Script::compile()
	{
		return new CompiledScript(new Executor(parser->astRoot));
	}

	InterpretableScript* Script::getInterpreter()
	{
		return new InterpretableScript(new Interpreter(parser->astRoot));
	}
	
	Script::~Script()
	{
		delete parser;
	}

	void Script::free()
	{
		delete this;
	}

}
