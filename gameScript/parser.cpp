#include "parser.h"
#include "ast.h"

namespace gs{

	extern void scan_string(const char* str);
	extern void scan_file(FILE* f);
	extern std::vector<NFunction*> *astRootBison;

	Parser::Parser(FILE* path)
	{
		scan_file(path);
		loadFunctions(astRootBison);
		astRootBison = nullptr;
	}

	Parser::Parser(const char* source)
	{
		scan_string(source);
		loadFunctions(astRootBison);
		astRootBison = nullptr;
	}

	void Parser::loadFunctions(std::vector<NFunction*>* funcs)
	{
		for (NFunction* f : *funcs)
		{
			astRoot.push_back(std::shared_ptr<NFunction>(f));
		}
	}
}