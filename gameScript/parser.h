#pragma once

#include "ast.h"
#include <memory>

namespace gs {

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
