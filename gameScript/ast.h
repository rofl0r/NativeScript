#pragma once
#include <vector>
#include <memory>

namespace gs {
	
	class NNumber;
	class NVariable;
	class NAssignment;
	class NBinaryOperator;
	class NCondition;
	class NForloop;
	class NFunctionCall;
	class NFunction;

	typedef std::shared_ptr<const NFunction> NFuncPtr;
	
	class AstVisitor {
	public:
		virtual void visit(const NNumber* node) = 0;
		virtual void visit(const NVariable* node) = 0;
		virtual void visit(const NAssignment* node) = 0;
		virtual void visit(const NBinaryOperator* node) = 0;
		virtual void visit(const NCondition* node) = 0;
		virtual void visit(const NForloop* node) = 0;
		virtual void visit(const NFunctionCall* node) = 0;
		virtual void visit(const NFunction* node) = 0;
	};

	class NExpression {
	public:
		virtual ~NExpression() {}
		virtual void accept(AstVisitor* visitor) const = 0;
	};

	class NNumber : public NExpression {
	public:
		double value;
		NNumber(double value) : value(value) { }
		void accept(AstVisitor* visitor) const override { visitor->visit(this); }
	};

	class NVariable : public NExpression {
	public:
		std::string name;
		NVariable(const std::string& name) : name(name) { }
		void accept(AstVisitor* visitor) const override { visitor->visit(this); }
	};

	class NAssignment : public NExpression {
	public:
		std::string name;
		NExpression& value;
		NAssignment(const std::string& name, NExpression& value) : name(name), value(value) { }
		void accept(AstVisitor* visitor) const override { visitor->visit(this); }
	};

	class NBinaryOperator : public NExpression {
	public:
		NExpression& lhs;
		NExpression& rhs;
		int op;
		NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
			lhs(lhs), rhs(rhs), op(op) { }
		void accept(AstVisitor* visitor) const override { visitor->visit(this); }
	};

	class NCondition : public NExpression {
	public:
		NExpression& cond;
		NExpression& iftrue;
		NExpression& iffalse;
		NCondition(NExpression& cond, NExpression& iftrue, NExpression& iffalse) :
			cond(cond), iftrue(iftrue), iffalse(iffalse) { }
		void accept(AstVisitor* visitor) const override { visitor->visit(this); }
	};

	class NForloop : public NExpression {
	public:
		std::string varName;
		NExpression& start;
		NExpression& end;
		NExpression& step;
		NExpression& body;
		NForloop(const std::string& varName, NExpression& start, NExpression& end, NExpression& step, NExpression& body) :
			varName(varName), start(start), end(end), step(step), body(body) { }
		void accept(AstVisitor* visitor) const override { visitor->visit(this); }
	};

	class NFunctionCall : public NExpression {
	public:
		std::string funcName;
		std::vector<NExpression*>& params;
		NFunctionCall(const std::string& funcName, std::vector<NExpression*>& params) :
			funcName(funcName), params(params) { }
		void accept(AstVisitor* visitor) const override { visitor->visit(this); }
	};

	class NFunction {
	public:
		std::string name;
		std::vector<std::string*> args;
		std::vector<NExpression*>* body;
		NExpression* returnExp;

		~NFunction() {}
		NFunction(const std::string& name, std::vector<std::string*>& args, NExpression* returnExp = nullptr, std::vector<NExpression*>* body = nullptr) :
			name(name), args(args), body(body), returnExp(returnExp) {}
		void accept(AstVisitor* visitor) const { visitor->visit(this); }
		bool hasBody() const { return returnExp != nullptr; };
	};	
}