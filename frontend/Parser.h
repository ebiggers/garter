#ifndef _GARTER_PARSER_H_
#define _GARTER_PARSER_H_

#include <frontend/Lexer.h>
#include <memory>
#include <vector>
#include <iostream>

namespace garter {

// Base class for all Abstract Syntax Tree (AST) nodes
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

// AST representing an entire program
class ProgramAST : public ASTBase {
public:
	// Top-level items (function definitions and statements) making up the
	// program
	std::vector<std::shared_ptr<ASTBase>> TopLevelItems;

	ProgramAST(const std::vector<std::shared_ptr<ASTBase>> &top_level_items)
		: TopLevelItems(top_level_items)
	{
	}

	void print(std::ostream & os) const;
};

// AST representing a function definition
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
class BreakStatementAST;
class ContinueStatementAST;
class ExpressionStatementAST;
class IfStatementAST;
class PassStatementAST;
class PrintStatementAST;
class ReturnStatementAST;
class WhileStatementAST;

// Visitor interface for statement AST nodes
class StatementASTVisitor {
public:
	virtual void visit(AssignmentStatementAST &) = 0;
	virtual void visit(BreakStatementAST &) = 0;
	virtual void visit(ContinueStatementAST &) = 0;
	virtual void visit(ExpressionStatementAST &) = 0;
	virtual void visit(IfStatementAST &) = 0;
	virtual void visit(PassStatementAST &) = 0;
	virtual void visit(PrintStatementAST &) = 0;
	virtual void visit(ReturnStatementAST &) = 0;
	virtual void visit(WhileStatementAST &) = 0;
};


// AST node representing a statement (abstract class subclassed by the actual
// statement types)
class StatementAST : public ASTBase {
public:
	virtual void acceptVisitor(StatementASTVisitor & v) = 0;
};

class VariableExpressionAST;
class ExpressionAST;

// Statement representing an assignment to a variable
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

// AST node representing a break statement
class BreakStatementAST : public StatementAST {
public:
	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

// AST node representing a break statement
class ContinueStatementAST : public StatementAST {
public:
	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

// AST node representing a statement consisting only of an expression whose
// value is ignored (for example a function call followed by a semicolon).
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

// AST node representing an 'if' statement, including the condition, body, any
// optional 'elif' clauses, and an optional 'else' clause.
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

// AST node representing an 'pass' statement, which does nothing.
class PassStatementAST : public StatementAST {
public:
	void print(std::ostream & os) const;
	void acceptVisitor(StatementASTVisitor & v) { v.visit(*this); }
};

// AST node representing a 'print' statement, which prints values to standard
// output.  (Note: this print *statement* potentially could be replaced with a
// built-in print *function*, like  in Python 3.)
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

// AST node representing a 'return' statement, which returns a value from a
// function.
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

// AST node representing a 'while' statement, including the condition and body.
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

// Visitor interface for expression AST nodes
class ExpressionASTVisitor {
public:
	virtual void visit(BinaryExpressionAST &) = 0;
	virtual void visit(CallExpressionAST &) = 0;
	virtual void visit(NumberExpressionAST &) = 0;
	virtual void visit(UnaryExpressionAST &) = 0;
	virtual void visit(VariableExpressionAST &) = 0;
};

// AST node representing an expression (abstract class subclassed by the actual
// expression types)
class ExpressionAST : public ASTBase {
public:
	virtual void acceptVisitor(ExpressionASTVisitor & v) = 0;
};

// AST node representing a binary operation performed on two sub-expressions,
// referred to as the left-hand side (LHS) and the right-hand side (RHS)
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

// AST node representing a function call
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

// AST node representing a numeric literal
class NumberExpressionAST : public ExpressionAST {
public:
	int32_t Number;
	NumberExpressionAST(int32_t number) : Number (number) {
	}
	void print(std::ostream & os) const;
	void acceptVisitor(ExpressionASTVisitor & v) { v.visit(*this); }
};

// AST node representing a unary expression
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

// AST node representing a reference to a variable (doesn't include function
// formal parameters)
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

// Parser for the garter language.  This class takes in a raw sequence of
// characters making up a garter program and returns an abstract syntax tree
// (AST) representing the program.  Internally, it uses the Lexer class to
// turn the raw input into Tokens before attempting to parse the higher-level
// expression, statement, and function definition constructs.  These
// higher-level constructs are recognized using a recursive decent parser.
class Parser {
private:
	Lexer TheLexer;
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
	std::unique_ptr<AssignmentStatementAST> parseAssignmentStatement();
	std::unique_ptr<BreakStatementAST>      parseBreakStatement();
	std::unique_ptr<ContinueStatementAST>   parseContinueStatement();
	std::unique_ptr<ExpressionStatementAST> parseExpressionStatement();
	std::unique_ptr<FunctionDefinitionAST>  parseFunctionDefinition();
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
			CurrentToken = TheLexer.getNextToken();
	}

	void nextTokenLookahead()
	{
		if (NextToken == nullptr)
			NextToken = TheLexer.getNextToken();
	}
public:
	// Create a Parser that reads a garter program from the specified
	// null-terminated string.
	Parser(const char *str) : TheLexer(str) { }

	// Create a Parser that reads a garter program from the specified input
	// stream.
	Parser(std::istream & is) : TheLexer(is) { }

	// Parse the input program and returns an abstract syntax tree
	// representing it, or nullptr if the input is not a valid program.
	std::unique_ptr<ProgramAST> parseProgram();

	// Parse the next top-level item (function definition or statement) in
	// the input program and returns an abstract syntax tree
	// (FunctionDefinitionAST or StatementAST) representing it, or nullptr
	// if the input is invalid or if the end of the input was reached.  The
	// latter two cases can be distinguished by subsequently calling
	// reachedEndOfFile().
	std::unique_ptr<ASTBase>    parseTopLevelItem();

	// Returns true iff the parser has attempted to read beyond the end of
	// the input.
	bool reachedEndOfFile() const { return TheLexer.reachedEndOfFile(); }
};

} // End garter namespace

#endif /* _GARTER_PARSER_H_ */
