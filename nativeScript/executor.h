#ifndef NS_EXECUTOR_H
#define NS_EXECUTOR_H

#include <map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include "ast.h"

namespace ns
{
	class Executor : public AstVisitor
	{
		std::map<std::string, llvm::Function*> functions;
		std::map<std::string, llvm::AllocaInst*> locals;
		llvm::Module* module;
		llvm::IRBuilder<> builder;
		llvm::ExecutionEngine* engine = nullptr;
		llvm::Value* result = nullptr;

		llvm::Value *error(const char *str);
		bool finalized = false;
		llvm::AllocaInst *createVariable(llvm::Function *f,	const std::string &name);
		llvm::AllocaInst *createVariable(const std::string &name); // usable only when the builder is set inside some fnc
		void declareFunction(const NFunction* node);
	public:
		Executor(const std::vector<NFuncPtr>& functionAst);
		void bindExternal(const char* name, void* fnc);
		void* getFunction(const char* name);
		void dumpIR();

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
