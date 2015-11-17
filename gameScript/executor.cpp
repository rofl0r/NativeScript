#include "executor.h"
#include "parser/tokens.h"
#include "include/gameScript.h"
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

namespace gs
{
	Executor::Executor(const std::vector<NFuncPtr>& functionAst)
		: builder(llvm::getGlobalContext())
	{
		InitializeNativeTarget();
		InitializeNativeTargetDisassembler();
		InitializeNativeTargetAsmPrinter();
		InitializeNativeTargetAsmParser();

		// Make the module, which holds all the code.
		auto m = make_unique<Module>("GameScript JIT", getGlobalContext());
		module = m.get();

		// COFF not supported by llvm on Win32
#if defined(GS_PLATFORM_WINDOWS) && !defined(_WIN64)
		auto triple = llvm::sys::getProcessTriple();
		module->setTargetTriple(triple + "-elf");
#endif

		engine = EngineBuilder(std::move(m)).create();

		// Create a new pass manager attached to it.
		auto TheFPM = llvm::make_unique<legacy::FunctionPassManager>(module);

		// Set up the optimizer pipeline.  Start with registering info about how the
		// target lays out data structures.
		module->setDataLayout(*engine->getDataLayout());;
		// Promote allocas to registers.
		TheFPM->add(createPromoteMemoryToRegisterPass());
		// Do simple "peephole" optimizations and bit-twiddling optzns.
		TheFPM->add(createInstructionCombiningPass());
		// Reassociate expressions.
		TheFPM->add(createReassociatePass());

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

		// generate code for all functions
		for (NFuncPtr f : functionAst)
		{
			f->accept(this);
			TheFPM->run(*(functions[f->name]));
		}
	}

	void Executor::bindExternal(const char* name, void* fnc)
	{
		if (finalized)
		{
			error("External functions cannot be bound anymore, script code has already been finalized.");
			return;
		}
		Function *f = functions[name];

		// make sure the function was declared, but not defined
		if (f == nullptr || !f->isDeclaration()) 
		{
			error("External function cannot be bound - it wasn't defined as external in script.");
			return;
		}
		engine->addGlobalMapping(f, fnc);
	}

	void* Executor::getFunction(const char* name)
	{
		if (!finalized)
		{
			finalized = true;
			// compile the stuff
			engine->finalizeObject();
		}
		llvm::Function *f = functions[name];
		return engine->getPointerToFunction(f);
	}

	void Executor::dumpIR()
	{
		module->dump();
	}

	Value *Executor::error(const char *str) {
		fprintf(stderr, "Code generation error: %s\n", str);
		return nullptr;
	}

	AllocaInst *Executor::createVariable(llvm::Function *f, const std::string &name)
	{
		IRBuilder<> varBuilder(&f->getEntryBlock(), f->getEntryBlock().begin());
		return varBuilder.CreateAlloca(Type::getDoubleTy(getGlobalContext()), 0, name.c_str());
	}

	AllocaInst *Executor::createVariable(const std::string &name)
	{
		return createVariable(builder.GetInsertBlock()->getParent(), name);
	}


	void Executor::visit(const NNumber* node)
	{
		result = ConstantFP::get(getGlobalContext(), APFloat(node->value));
	}

	void Executor::visit(const NVariable* node)
	{
		result = locals[node->name];
		if (!result) {
			error("Unknown variable name");
		}

		result = builder.CreateLoad(result, node->name.c_str());
	}

	void Executor::visit(const NAssignment* node)
	{
		AllocaInst* var = locals[node->name];

		// lazy allocation
		if (!var) {
			var = createVariable(node->name);
			locals[node->name] = var;
		}
		node->value.accept(this);

		builder.CreateStore(result, var);

		// result is already set
	}

	void Executor::visit(const NBinaryOperator* node)
	{
		node->lhs.accept(this);
		Value *l = result;
		node->rhs.accept(this);
		Value *r = result;
		if (!l || !r) return;

		switch (node->op) {
		case '+':
			result = builder.CreateFAdd(l, r, "add");
			break;
		case '-':
			result = builder.CreateFSub(l, r, "sub");
			break;
		case '*':
			result = builder.CreateFMul(l, r, "mul");
			break;
		case '<':
			l = builder.CreateFCmpOLT(l, r, "cmpLT");
			// Convert bool 0/1 to double 0.0 or 1.0
			result = builder.CreateUIToFP(l, Type::getDoubleTy(getGlobalContext()), "boolToD");
			break;
		case TEQUAL:
			l = builder.CreateFCmpOEQ(l, r, "cmpEQ");
			// Convert bool 0/1 to double 0.0 or 1.0
			result = builder.CreateUIToFP(l, Type::getDoubleTy(getGlobalContext()), "boolToD");
			break;
		default:
			result = error("Invalid binary operator");
		}
	}

	void Executor::visit(const NCondition* node)
	{
		node->cond.accept(this);
		if (!result) return;

		// Convert condition to a bool by comparing equal to 0.0.
		result = builder.CreateFCmpONE(result, ConstantFP::get(getGlobalContext(), APFloat(0.0)), "ifcond");

		Function *f = builder.GetInsertBlock()->getParent();

		// Create blocks for the then and else cases.  Insert the 'then' block at the
		// end of the function.
		BasicBlock *tBlock = BasicBlock::Create(getGlobalContext(), "iftrue", f);
		BasicBlock *fBlock = BasicBlock::Create(getGlobalContext(), "iffalse");
		BasicBlock *mergeBlock = BasicBlock::Create(getGlobalContext(), "ifmerge");

		builder.CreateCondBr(result, tBlock, fBlock);

		// Emit iftrue value.
		builder.SetInsertPoint(tBlock);

		node->iftrue.accept(this);
		if (!result) return;
		Value* iftVal = result;

		builder.CreateBr(mergeBlock);
		// Codegen of 'iftrue' can change the current block, update tBlock for the PHI.
		tBlock = builder.GetInsertBlock();


		// Emit iffalse block.
		f->getBasicBlockList().push_back(fBlock);
		builder.SetInsertPoint(fBlock);

		node->iffalse.accept(this);
		if (!result) return;

		builder.CreateBr(mergeBlock);
		// codegen of 'iffalse' can change the current block, update fBlock for the PHI.
		fBlock = builder.GetInsertBlock();


		// Emit merge block.
		f->getBasicBlockList().push_back(mergeBlock);
		builder.SetInsertPoint(mergeBlock);
		PHINode *PN = builder.CreatePHI(Type::getDoubleTy(getGlobalContext()), 2, "ifMerge");

		PN->addIncoming(iftVal, tBlock);
		PN->addIncoming(result, fBlock);
		
		result = PN;
	}

	void Executor::visit(const NForloop* node)
	{
		// TODO: this is DO WHILE now, (condition is not checked for the first execution of body)
		// probably just add comparison instead of explicit fall trough from current block to loop block

		AllocaInst *forVar = createVariable(node->varName);

		node->start.accept(this);
		builder.CreateStore(result, forVar);

		// Make the new basic block for the loop header, inserting after current
		// block.
		Function *f = builder.GetInsertBlock()->getParent();
		BasicBlock *loopBlock = BasicBlock::Create(getGlobalContext(), "loop", f);

		// Insert an explicit fall through from the current block to the LoopBB.
		builder.CreateBr(loopBlock);

		// Start insertion in LoopBB.
		builder.SetInsertPoint(loopBlock);

		// Don't allow shading variables
		if (locals[node->varName]) {
			result = error("For loop variable name already defined."); 
			return;
		}
		locals[node->varName] = forVar;

		// Emit the body of the loop.  This, like any other expr, can change the
		// current BB.  Note that we ignore the value computed by the body, but don't
		// allow an error.
		node->body.accept(this);
		if (!result) return;

		// Emit the step value.
		//Value *stepVal = nullptr;
		//if (step) {
		node->step.accept(this);
		if (!result) return;
		Value *stepVal = result;
		
		//}
		//else {
		// If not specified, use 1.0.
		//	StepVal = ConstantFP::get(getGlobalContext(), APFloat(1.0));
		//}
		Value *CurVar = builder.CreateLoad(forVar);
		Value *NextVar = builder.CreateFAdd(CurVar, stepVal, "nextvar");
		builder.CreateStore(NextVar, forVar);

		// Compute the end condition.
		node->end.accept(this);
		if (!result) return;
		Value *EndCond = result;

		// Convert condition to a bool by comparing equal to 0.0.
		EndCond = builder.CreateFCmpONE(
			EndCond, ConstantFP::get(getGlobalContext(), APFloat(0.0)), "loopcond");

		// Create the "after loop" block and insert it.
		BasicBlock *AfterBB =
			BasicBlock::Create(getGlobalContext(), "afterloop", f);

		// Insert the conditional branch into the end of LoopEndBB.
		builder.CreateCondBr(EndCond, loopBlock, AfterBB);

		// Any new code will be inserted in AfterBB.
		builder.SetInsertPoint(AfterBB);

		// for expr always returns 0.0.
		result = Constant::getNullValue(Type::getDoubleTy(getGlobalContext()));
	}

	void Executor::visit(const NFunctionCall* node)
	{
		Function *f = module->getFunction(node->funcName.c_str());
		if (f == NULL) {
			result = error("Calling unknown function.");
		}
		if (f->arg_size() != node->params.size()) {
			result = error("Incorrect # arguments passed to function call");
			return;
		}
		std::vector<Value *> paramVal;
		for (unsigned i = 0, e = node->params.size(); i != e; ++i) {
			node->params[i]->accept(this);
			if (!result) return;
			paramVal.push_back(result);
		}

		result = builder.CreateCall(f, paramVal, "call");
	}

	void Executor::visit(const NFunction* node)
	{
		// if the function was already defined, throw an error
		if (functions[node->name] != nullptr && !functions[node->name]->isDeclaration())
		{
			result = error("Attempt to redefine function");
			return;
		}

		Function *f;

		// if the function wasn't declared yet, declare it
		if (functions[node->name] == nullptr) {
			std::vector<Type *> argTypes(node->args.size(), Type::getDoubleTy(getGlobalContext()));
			FunctionType *ftype = FunctionType::get(Type::getDoubleTy(getGlobalContext()), argTypes, false);
			f = Function::Create(ftype, Function::ExternalLinkage, node->name.c_str(), module);

			functions[node->name] = f;
		} else
		{
			// Otherwise use previous declaration
			f = functions[node->name];
		}

		// if this is definition, create the body
		if (node->hasBody()) {
			// Add a basic block to the FooF function.
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "EntryBlock", f);

			// Tell the basic block builder to attach itself to the new basic block
			builder.SetInsertPoint(BB);

			Function::arg_iterator AI = f->arg_begin();
			for (unsigned Idx = 0, e = node->args.size(); Idx != e; ++Idx, ++AI) {
				// Create an alloca for this variable.
				AllocaInst *parVar = createVariable(*node->args[Idx]);

				// Store the initial value into the alloca.
				builder.CreateStore(AI, parVar);

				// Add arguments to variable symbol table.
				locals[*node->args[Idx]] = parVar;
			}

			for (NExpression* e : *node->body)
			{
				e->accept(this);
			}
			
			node->returnExp->accept(this);

			// Create the return instruction and add it to the basic block.
			builder.CreateRet(result);

			verifyFunction(*f);
		}
	}

}