#ifndef NS_PARSER_H
#define NS_PARSER_H

#include <memory>

#include "ast.h"

namespace ns {

	class Parser
	{
		void loadFunctions(std::vector<NFunction*>* funcs);
	public:
		std::vector<NFuncPtr> astRoot;

		// after constructor run, the FILE can be closed
		Parser(FILE* path);
		Parser(const char* source);
	};
}

#endif
