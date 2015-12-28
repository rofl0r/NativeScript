#ifndef NS_INTERPRETER_H
#define NS_INTERPRETER_H

#include <map>

#include "include/nativeScript.h"
#include "ast.h"

namespace ns {
	class Interpreter : public AstVisitor
	{
	private:
		std::map<std::string, NFuncPtr> functions;
		std::map<std::string, ExternalFunction> externals;
		std::map<std::string, double> locals;
		double result;
	public:
		Interpreter(const std::vector<NFuncPtr>& functions);
		double runFunction(const char* name, const double* params);
		void bindExternal(const char* name, ExternalFunction fnc);
		
		void visit(const NNumber* node) override;
		void visit(const NVariable* node) override;
		void visit(const NAssignment* node) override;
		void visit(const NBinaryOperator* node) override;
		void visit(const NCondition* node) override;
		void visit(const NForloop* node) override;
		void visit(const NFunctionCall* node) override;
		void visit(const NFunction* node) override;
	};
}

#endif
