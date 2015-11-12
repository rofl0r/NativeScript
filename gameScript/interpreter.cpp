#include "parser/bison.h"
#include "parser.h"
#include "interpreter.h"

namespace gs {

	Interpreter::Interpreter(const std::vector<NFuncPtr>& fncs)  {
		for (NFuncPtr f : fncs)
		{
			functions[f->name] = f;
		}
	}

	double Interpreter::runFunction(const char* name, const double* params)
	{
		const NFuncPtr f = functions[name];
		// TODO: if not found throw error about calling undefined function
		if (f->hasBody()) {
			// TODO: solve overriding current locals (save and restore after run)
			for (std::string* arg : f->args)
			{
				locals[*arg] = *(params++);
			}
			f->body->accept(this);
			locals.clear();
			// TODO: clear only when called externally, otherwise restore
			return result;
		}
		else
		{
			return externals[name](f->args.size(), params);
			// TODO: if not found, throw error about external not being bound
		}
	}

	void Interpreter::bindExternal(const char* name, ExternalFunction fnc)
	{
		externals[name] = fnc;
	}

	void Interpreter::visit(const NNumber* node)
	{
		result = node->value;
	}

	void Interpreter::visit(const NIdentifier* node)
	{
		result = locals[node->name];
	}

	void Interpreter::visit(const NBinaryOperator* node)
	{
		node->lhs.accept(this);
		double l = result;
		
		node->rhs.accept(this);
		double r = result;

		switch (node->op) {
		case '+': result = l + r; break;
		case '-': result = l - r; break;
		case '*': result = l * r; break;
		case '<': result = l < r; break;
		case TEQUAL: result = l == r; break;
		
		default:
			printf("Invalid binary operator %c", node->op);
			return;
		}
	}
	
	void Interpreter::visit(const NCondition* node)
	{
		node->cond.accept(this);
		if (result) {
			node->iftrue.accept(this);
		} 
		else
		{
			node->iffalse.accept(this);
		}

		// result is set
	}

	void Interpreter::visit(const NForloop* node)
	{
		// evaluate init value of loop variable
		node->start.accept(this);
		double loopVar = result;

		// save variable shadowed by for variable
		bool oldValExists = false;
		double oldVal = 0;
		if (locals.find(node->varName) != locals.end())
		{
			oldValExists = true;
			oldVal = locals[node->varName];
		}

		// put loop variable into context
		locals[node->varName] = loopVar;

		// as long as condition holds
		node->end.accept(this);
		while (result != 0)
		{
			// evaluate body
			node->body.accept(this);
			// update loop variable by step
			node->step.accept(this);
			loopVar += result;
			locals[node->varName] = loopVar;
			// check condition again
			node->end.accept(this);
		};

		if (oldValExists)
		{
			locals[node->varName] = oldVal;
		}
		else
		{
			locals.erase(node->varName);
		}

		result = 0;
	}

	void Interpreter::visit(const NFunctionCall* node)
	{
		double* vals = new double[node->params.size()];
		int ptr = 0;
		for (NExpression* e : node->params)
		{
			e->accept(this);
			vals[ptr++] = result;
		}
		result = runFunction(node->funcName.c_str(), vals);

		delete[] vals;
	}

	void Interpreter::visit(const NFunction* node)
	{
		// TODO
	}
}