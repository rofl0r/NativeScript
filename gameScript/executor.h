#pragma once

#include "ast.h"
#include <map>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

namespace gs
{
	class Executor : public AstVisitor
	{
		std::map<std::string, llvm::Function*> functions;
		std::map<std::string, llvm::Value*> locals;
		std::unique_ptr<llvm::Module> module;
		llvm::IRBuilder<> builder;
		llvm::ExecutionEngine* engine = nullptr;
		llvm::Value* result = nullptr;

		llvm::Value *error(const char *str);
		bool finalized = false;
	public:
		Executor(const std::vector<NFuncPtr>& functionAst);
		void bindExternal(const char* name, void* fnc);
		void* getFunction(const char* name);
		void dumpCode();

		void visit(const NNumber* node) override;
		void visit(const NIdentifier* node) override;
		void visit(const NBinaryOperator* node) override;
		void visit(const NCondition* node) override;
		void visit(const NForloop* node) override;
		void visit(const NFunctionCall* node) override;
		void visit(const NFunction* node) override;
	};
}