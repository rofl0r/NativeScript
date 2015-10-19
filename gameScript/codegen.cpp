#include "ast.h"
#include "codegen.h"
#include "include/gameScript.h"
#include <set>
#include <vector>
#include "llvm/IR/Verifier.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Transforms/Scalar.h"

using namespace llvm;

namespace gs {

	void* CodeGen::getFunction(const char* name)
	{
		if (!compiled)
		{
			compile(false);
		}
		llvm::Function *f = (*functions)[name];
		return engine->getPointerToFunction(f);
	}

	void CodeGen::compile(bool dumpCode)
	{
		CodeGenContext codeGen;
		functions = codeGen.generateIR(root);
		engine = codeGen.compile(dumpCode);
		compiled = true;
	}

	double CodeGen::interpretFunction(const char* name, double* params)
	{
		return (*rawFunctions)[name]->evaluate(params);
	}

	CodeGen::CodeGen(std::vector<gs::NFunction*>* root, bool dumpCode) : root(root)
	{
		if (dumpCode)
		{
			compile(true);
		}
		rawFunctions = new std::map < std::string, NFunction* > ;
		for (NFunction *f : *root)
		{
			(*rawFunctions)[f->name] = f;
		}	
	}

	CodeGen::~CodeGen()
	{
		delete functions;
		delete engine;
		delete rawFunctions;
		delete root;
	}

	Value *CodeGenContext::error(const char *str) {
		fprintf(stderr, "Code generation error: %s\n", str);
		return nullptr;
	}

	std::map<std::string, llvm::Function*> *CodeGenContext::generateIR(std::vector<gs::NFunction*>* root)
	{
		InitializeNativeTarget();
		InitializeNativeTargetDisassembler();
		InitializeNativeTargetAsmPrinter();
		InitializeNativeTargetAsmParser();

		// Make the module, which holds all the code.
		module = make_unique<Module>("GameScript JIT", getGlobalContext());

		// COFF not supported on Win32
#if defined(GS_PLATFORM_WINDOWS) && !defined(_WIN64)
		auto triple = llvm::sys::getProcessTriple();
		module->setTargetTriple(triple + "-elf");
#endif

		
		// Create a new pass manager attached to it.
		auto TheFPM = llvm::make_unique<legacy::FunctionPassManager>(module.get());

		// Provide basic AliasAnalysis support for GVN.
		TheFPM->add(createBasicAliasAnalysisPass());
		//NOT WORKING HERE - BECAUSE OF FLOAT?// Do simple "peephole" optimizations and bit-twiddling optzns.
		TheFPM->add(createInstructionCombiningPass());
		// Reassociate expressions.
		TheFPM->add(createReassociatePass());
		// Eliminate Common SubExpressions.
		TheFPM->add(createGVNPass());
		// Simplify the control flow graph (deleting unreachable blocks, etc).
		TheFPM->add(createCFGSimplificationPass());

		TheFPM->doInitialization();

		auto functions = new std::map<std::string, llvm::Function*>;

		// Now we're going to create function `foo', which returns an int and takes no
		// arguments.
		for (NFunction *f : *root)
		{
			Function* fIR = f->codeGen(*this);
			TheFPM->run(*fIR);
			(*functions)[f->name] = fIR;
		}
		
		return functions;
	}

	ExecutionEngine* CodeGenContext::compile(bool dumpCode)
	{
		if (dumpCode)
		{
			module->dump();
		}

		ExecutionEngine* EE = EngineBuilder(std::move(module)).create();
		// compile the stuff
		EE->finalizeObject();
		return EE;
	}

	Function *NFunction::codeGen(CodeGenContext& context) {
		std::vector<Type *> argTypes(args.size(), Type::getDoubleTy(getGlobalContext()));
		FunctionType *ftype = FunctionType::get(Type::getDoubleTy(getGlobalContext()), argTypes, false);
		//Function * f = cast<Function>(context.module.get()->getOrInsertFunction(name, ftype));
		Function *f = Function::Create(ftype, GlobalValue::InternalLinkage, name.c_str(), context.module.get());
		unsigned Idx = 0;
		context.namedValues.clear();
		for (auto &arg : f->args()) {
			arg.setName(args[Idx++]->c_str());
			context.namedValues[arg.getName()] = &arg;
		}

		// Add a basic block to the FooF function.
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "EntryBlock", f);

		// Tell the basic block builder to attach itself to the new basic block
		context.builder.SetInsertPoint(BB);

		Value* result = body.codeGen(context);

		// Create the return instruction and add it to the basic block.
		context.builder.CreateRet(result);

		verifyFunction(*f);

		return f;
	}

	Value *NNumber::codeGen(CodeGenContext& context) {
		return ConstantFP::get(getGlobalContext(), APFloat(value));
	}

	Value *NIdentifier::codeGen(CodeGenContext& context) {
		Value *v = context.namedValues[name];
		if (!v) {
			context.error("Unknown variable name");
		}
		return v;
	}

	Value *NBinaryOperator::codeGen(CodeGenContext& context) {
		Value *l = lhs.codeGen(context);
		Value *r = rhs.codeGen(context);
		if (!l || !r)
			return nullptr;

		switch (op) {
		case '+':
			return context.builder.CreateFAdd(l, r, "add");
		case '-':
			return context.builder.CreateFSub(l, r, "sub");
		case '*':
			return context.builder.CreateFMul(l, r, "mul");
		default:
			return context.error("Invalid binary operator");
		}
	}

	double NBinaryOperator::evaluate(std::map<std::string, double>* locals)
	{
		switch (op) {
		case '+':
			return lhs.evaluate(locals) + rhs.evaluate(locals);
		case '-':
			return lhs.evaluate(locals) - rhs.evaluate(locals);
		case '*':
			return lhs.evaluate(locals) * rhs.evaluate(locals);
		default:
			printf("Invalid binary operator");
			return 0;
		}
	}

	double NFunction::evaluate(double* locals)
	{
		int cnt = 0;
		for (std::string* par : args)
		{
			evalVals[*par] = locals[cnt++];
		}
		return body.evaluate(&evalVals);
	}

}