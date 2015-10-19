#pragma once
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include "llvm/IR/LLVMContext.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>

namespace gs {

	class CodeGen
	{
	private:
		std::map<std::string, llvm::Function*> *functions;
		llvm::ExecutionEngine *engine;
		std::map<std::string, gs::NFunction*> *rawFunctions;
		std::vector<gs::NFunction*>* root;
		bool compiled = false; // lazy compilation if only interpreter is used
		void compile(bool dumpCode);
	public:
		CodeGen(std::vector<gs::NFunction*>* root, bool dump);
		void* getFunction(const char* name);
		double interpretFunction(const char* name, double* params);
		~CodeGen();
	};

	class CodeGenContext
	{
	public:
		std::map<std::string, llvm::Value*> namedValues;
		std::unique_ptr<llvm::Module> module;
		llvm::IRBuilder<> builder;

		CodeGenContext() : builder(llvm::getGlobalContext()) {}
		~CodeGenContext() {}
		llvm::Value *error(const char *str);
		std::map<std::string, llvm::Function*> *generateIR(std::vector<gs::NFunction*>* root);
		llvm::ExecutionEngine* compile(bool dumpCode);
	};
}

