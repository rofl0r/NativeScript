#include "include/gameScript.h"
#include "ast.h"
#include "codegen.h"
#include <iostream>

extern FILE *yyin;
extern int yyparse();
extern std::vector<gs::NFunction*> *astRoot;

namespace gs {

	extern void scan_string(const char* str);

	Script::Script(FILE *file, bool dumpCode)
	{
		yyin = file;
		do {
			yyparse();
		} while (!feof(yyin));
		codeGen = new CodeGen(astRoot,dumpCode);
		astRoot = nullptr;
	}

	Script::Script(const char* source, bool dumpCode)
	{
		scan_string(source);
		codeGen = new CodeGen(astRoot, dumpCode);
		astRoot = nullptr;
	}

	void* Script::getFunction(const char* name)
	{
		return codeGen->getFunction(name);
	}

	double Script::interpretFunction(const char* name, double* params)
	{
		return codeGen->interpretFunction(name, params);
	}

	Script::~Script()
	{
		delete codeGen;
	}

}
