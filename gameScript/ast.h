#pragma once
#include <llvm/IR/Value.h>
#include <map>
#include <vector>

namespace gs {

	class CodeGenContext;

	class NExpression {
	public:
		virtual ~NExpression() {}
		virtual llvm::Value* codeGen(CodeGenContext& context) = 0;
		virtual double evaluate(std::map<std::string,double>* locals) = 0;
	};

	class NNumber : public NExpression {
	public:
		double value;
		NNumber(double value) : value(value) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual double evaluate(std::map<std::string, double>* locals) { return value; }
	};

	class NIdentifier : public NExpression {
	public:
		std::string name;
		NIdentifier(const std::string& name) : name(name) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual double evaluate(std::map<std::string, double>* locals) { return (*locals)[name]; }
	};

	class NBinaryOperator : public NExpression {
	public:
		int op;
		NExpression& lhs;
		NExpression& rhs;
		NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
			lhs(lhs), rhs(rhs), op(op) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual double evaluate(std::map<std::string, double>* locals);
	};

	class NFunction {
	public:
		std::string name;
		std::vector<std::string*> args;
		NExpression& body;
		std::map<std::string, double> evalVals;
		NFunction(const std::string& name, std::vector<std::string*>& args, NExpression& body) :
			name(name), args(args), body(body) { }
		llvm::Function* codeGen(CodeGenContext& context);
		virtual double evaluate(double* locals);
	};
}