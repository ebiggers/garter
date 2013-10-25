#ifndef _GARTER_PARSER_H_
#define _GARTER_PARSER_H_

#include <frontend/Lexer.h>
#include <memory>
#include <vector>
#include <iostream>

namespace garter {

class ASTBase {
public:
	virtual ~ASTBase() { }

	virtual void print(std::ostream & os) const = 0;

	friend std::ostream & operator<<(std::ostream & os, const ASTBase & base)
	{
		base.print(os);
		return os;
	}
};

class StatementAST;

class ProgramAST : public ASTBase {
public:
	std::vector<std::shared_ptr<ASTBase>> TopLevelItems;
	ProgramAST(const std::vector<std::shared_ptr<ASTBase>> &top_level_items)
		: TopLevelItems(top_level_items)
	{
	}

	void print(std::ostream & os) const;
};

class FunctionDefinitionAST : public ASTBase {
public:
	std::string Name;
	std::vector<std::string> Parameters;
	std::vector<std::shared_ptr<StatementAST>> Body;
	bool IsExtern;

	FunctionDefinitionAST(const std::string & name,
			      const std::vector<std::string> & parameters,
			      const std::vector<std::shared_ptr<StatementAST>> & body,
			      bool is_extern = false)
		: Name(name), Parameters(parameters), Body(body), IsExtern(is_extern)
	{
	}

	void print(std::ostream & os) const;
};

class AssignmentStatementAST;
class ExpressionStatementAST;
class IfStatementAST;
class PassStatementAST;
class PrintStatementAST;
class ReturnStatementAST;
class WhileStatementAST;

class StatementASTVisitor {
public:
	virtual void visit(AssignmentStatementAST &) = 0;
	virtual void visit(ExpressionStatementAST &) = 0;
	virtual void visit(IfStatementAST &) = 0;
	virtual void visit(PassStatementAST &) = 0;
	virtual void visit(PrintStatementAST &) = 0;
	virtual void visit(ReturnStatementAST &) = 0;
	virtual void visit(WhileStatementAST &) = 0;
};


class StatementAST : public ASTBase {
public:
	virtual void acceptVisitor(StatementASTVisitor & v) = 0;
};

class VariableExpressionAST;
class ExpressionAST;

class AssignmentStatementAST : public StatementAST {
public:
	std::shared_ptr<VariableExpressionAST> Variable;
	std::shared_ptr<ExpressionAST> Expression;

	AssignmentStatementAST(std::shared_ptr<VariableExpressionAST> lhs,
			       std::shared_ptr<ExpressionAST> rhs)
		: Variable(lhs), Expression(rhs)
	{
	}

	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

class ExpressionStatementAST : public StatementAST {
public:
	std::shared_ptr<ExpressionAST> Expression;

	ExpressionStatementAST(std::shared_ptr<ExpressionAST> expression)
		: Expression(expression)
	{
	}
	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

class IfStatementAST : public StatementAST {
public:
	class ElifClause {
	public:
		std::shared_ptr<ExpressionAST> Condition;
		std::vector<std::shared_ptr<StatementAST>> Body;

		ElifClause(std::shared_ptr<ExpressionAST> condition,
			   const std::vector<std::shared_ptr<StatementAST>> & body)
			: Condition(condition),
			  Body(body)
		{
		}

		void print(std::ostream & os) const;
	};

	std::shared_ptr<ExpressionAST> Condition;
	std::vector<std::shared_ptr<StatementAST>> Body;
	std::vector<std::shared_ptr<ElifClause>> ElifClauses;
	std::vector<std::shared_ptr<StatementAST>> ElseBody;

	IfStatementAST(std::shared_ptr<ExpressionAST> condition,
		       const std::vector<std::shared_ptr<StatementAST>> & body,
		       const std::vector<std::shared_ptr<ElifClause>> & elif_clauses,
		       const std::vector<std::shared_ptr<StatementAST>> & else_body)
		: Condition(condition),
		  Body(body),
		  ElifClauses(elif_clauses),
		  ElseBody(else_body)
	{
	}

	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

class PassStatementAST : public StatementAST {
public:
	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

class PrintStatementAST : public StatementAST {
public:
	std::vector<std::shared_ptr<ExpressionAST>> Arguments;

	PrintStatementAST(const std::vector<std::shared_ptr<ExpressionAST>> & arguments)
		: Arguments(arguments)
	{
	}
	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

class ReturnStatementAST : public StatementAST {
public:
	std::shared_ptr<ExpressionAST> Expression;
	ReturnStatementAST(std::shared_ptr<ExpressionAST> expression)
		: Expression(expression)
	{
	}

	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

class WhileStatementAST : public StatementAST {
public:
	std::shared_ptr<ExpressionAST> Condition;
	std::vector<std::shared_ptr<StatementAST>> Body;

	WhileStatementAST(std::shared_ptr<ExpressionAST> condition,
			  const std::vector<std::shared_ptr<StatementAST>> & body)
		: Condition(condition),
		  Body(body)
	{
	}
	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

class BinaryExpressionAST;
class CallExpressionAST;
class NumberExpressionAST;
class UnaryExpressionAST;
class VariableExpressionAST;

class ExpressionASTVisitor {
public:
	virtual void visit(BinaryExpressionAST &) = 0;
	virtual void visit(CallExpressionAST &) = 0;
	virtual void visit(NumberExpressionAST &) = 0;
	virtual void visit(UnaryExpressionAST &) = 0;
	virtual void visit(VariableExpressionAST &) = 0;
};

class ExpressionAST : public ASTBase {
public:
	virtual void acceptVisitor(ExpressionASTVisitor & v) = 0;
};

class BinaryExpressionAST : public ExpressionAST {
public:
	enum BinaryOp {
		None = -1,
		Or = 0,
		And,
		LessThan,
		GreaterThan,
		LessThanOrEqualTo,
		GreaterThanOrEqualTo,
		EqualTo,
		NotEqualTo,
		Add,
		Subtract,
		Multiply,
		Divide,
		Modulo,
		Exponentiate,
		In,
		NotIn,
	};

	enum BinaryOp Op;
	std::shared_ptr<ExpressionAST> LHS, RHS;

	BinaryExpressionAST(enum BinaryOp op,
			    std::shared_ptr<ExpressionAST> lhs,
			    std::shared_ptr<ExpressionAST> rhs)
		: Op(op), LHS(lhs), RHS(rhs)
	{
	}

	const char *getOpStr() const;
	void print(std::ostream & os) const;
	void acceptVisitor(ExpressionASTVisitor & v) { v.visit(*this); }
};

class CallExpressionAST : public ExpressionAST {
public:
	std::string Callee;
	std::vector<std::shared_ptr<ExpressionAST>> Arguments;

	CallExpressionAST(const std::string &callee,
			  const std::vector<std::shared_ptr<ExpressionAST>> &arguments)
		: Callee(callee), Arguments(arguments)
	{
	}
	void print(std::ostream & os) const;
	void acceptVisitor(ExpressionASTVisitor & v) { v.visit(*this); }
};

class NumberExpressionAST : public ExpressionAST {
public:
	int32_t Number;
	NumberExpressionAST(int32_t number) : Number (number) {
	}
	void print(std::ostream & os) const;
	void acceptVisitor(ExpressionASTVisitor & v) { v.visit(*this); }
};

class UnaryExpressionAST : public ExpressionAST {
public:
	enum UnaryOp {
		Not,
		Minus,
		Plus,
	};

	enum UnaryOp Op;
	std::shared_ptr<ExpressionAST> Expression;

	UnaryExpressionAST(enum UnaryOp op,
			   std::shared_ptr<ExpressionAST> expression)
		: Op(op), Expression(expression)
	{
	}
	const char *getOpStr() const;
	void print(std::ostream & os) const;
	void acceptVisitor(ExpressionASTVisitor & v) { v.visit(*this); }
};

class VariableExpressionAST : public ExpressionAST {
public:
	std::string Name;
	VariableExpressionAST(const std::string &name)
		: Name(name)
	{
	}

	void print(std::ostream & os) const;
	void acceptVisitor(ExpressionASTVisitor & v) { v.visit(*this); }
};

class Parser {
private:
	Lexer Lexer;
	std::unique_ptr<Token> CurrentToken;
	std::unique_ptr<Token> NextToken;

	BinaryExpressionAST::BinaryOp		currentMultiplicationOperator();
	BinaryExpressionAST::BinaryOp		currentAdditionOperator();
	BinaryExpressionAST::BinaryOp		currentComparisonOperator();
	std::unique_ptr<ExpressionAST>          parseNumberExpression();
	std::unique_ptr<ExpressionAST>          parseIdentifierExpression();
	std::unique_ptr<ExpressionAST>          parseParenthesizedExpression();
	std::unique_ptr<ExpressionAST>          parsePrimaryExpression();
	std::unique_ptr<ExpressionAST>          parsePowerExpression();
	std::unique_ptr<ExpressionAST>          parseUnaryExpression();
	std::unique_ptr<ExpressionAST>          parseMultiplicationExpression();
	std::unique_ptr<ExpressionAST>          parseAdditionExpression();
	std::unique_ptr<ExpressionAST>          parseComparisonExpression();
	std::unique_ptr<ExpressionAST>          parseNotExpression();
	std::unique_ptr<ExpressionAST>          parseAndExpression();
	std::unique_ptr<ExpressionAST>          parseExpression();
	std::unique_ptr<ExpressionStatementAST> parseExpressionStatement();
	std::unique_ptr<FunctionDefinitionAST>  parseFunctionDefinition();
	std::unique_ptr<AssignmentStatementAST> parseAssignmentStatement();
	std::unique_ptr<IfStatementAST>         parseIfStatement();
	std::unique_ptr<PassStatementAST>       parsePassStatement();
	std::unique_ptr<PrintStatementAST>      parsePrintStatement();
	std::unique_ptr<ReturnStatementAST>     parseReturnStatement();
	std::unique_ptr<WhileStatementAST>      parseWhileStatement();
	std::unique_ptr<StatementAST>           parseStatement();

	void nextToken()
	{
		if (NextToken)
			CurrentToken = std::move(NextToken);
		else
			CurrentToken = Lexer.getNextToken();
	}

	void nextTokenLookahead()
	{
		if (NextToken == nullptr)
			NextToken = Lexer.getNextToken();
	}
public:
	Parser(const char *str) : Lexer(str) { }

	Parser(std::istream & is) : Lexer(is) { }

	~Parser() { }

	std::unique_ptr<ProgramAST> parseProgram();
	std::unique_ptr<ASTBase>    parseTopLevelItem();

	bool reachedEndOfFile() const { return Lexer.reachedEndOfFile(); }
};

} // End garter namespace

#endif /* _GARTER_PARSER_H_ */
