#include <frontend/Lexer.h>
#include <memory>

namespace garter {

class AST {
public:
	virtual ~AST() {
	}
};

class ProgramAST : public AST {
	std::vector<std::shared_ptr<AST>> TopLevelItems;
public:
	ProgramAST(const std::vector<std::shared_ptr<AST>> &top_level_items)
		: TopLevelItems(top_level_items)
	{
	}
};

class ExpressionAST : public AST {
};

class StatementAST : public AST {
};

class VariableExpressionAST : public ExpressionAST {
	std::string Name;
public:
	VariableExpressionAST(const std::string &name)
		: Name(name)
	{
	}
};

class AssignmentStatementAST : public StatementAST {
	std::shared_ptr<VariableExpressionAST> Variable;
	std::shared_ptr<ExpressionAST> Expression;

public:
	AssignmentStatementAST(std::shared_ptr<VariableExpressionAST> lhs,
			       std::shared_ptr<ExpressionAST> rhs)
		: Variable(lhs), Expression(rhs)
	{
	}
};

class PassStatementAST : public StatementAST {
};

class ReturnStatementAST : public StatementAST {
	std::shared_ptr<ExpressionAST> Expression;
public:
	ReturnStatementAST(std::shared_ptr<ExpressionAST> expression)
		: Expression(expression)
	{
	}
};

class IfStatementAST : public StatementAST {
public:
	class ElifClause {
	private:
		std::shared_ptr<ExpressionAST> Condition;
		std::vector<std::shared_ptr<StatementAST>> Body;

	public:
		ElifClause(std::shared_ptr<ExpressionAST> condition,
			   const std::vector<std::shared_ptr<StatementAST>> & body)
			: Condition(condition),
			  Body(body)
		{
		}
	};

private:
	std::shared_ptr<ExpressionAST> Condition;
	std::vector<std::shared_ptr<StatementAST>> Statements;
	std::vector<std::shared_ptr<ElifClause>> ElifClauses;
	std::vector<std::shared_ptr<StatementAST>> ElseStatements;

public:
	IfStatementAST(ExpressionAST *condition,
		       const std::vector<std::shared_ptr<StatementAST>> & statements,
		       const std::vector<std::shared_ptr<ElifClause>> & elif_clauses,
		       const std::vector<std::shared_ptr<StatementAST>> & else_statements)
		: Condition(condition),
		  Statements(statements),
		  ElifClauses(elif_clauses),
		  ElseStatements(else_statements)
	{
	}
};

class WhileStatementAST : public StatementAST {
	std::shared_ptr<ExpressionAST> Condition;
	std::vector<std::shared_ptr<StatementAST>> Body;

public:
	WhileStatementAST(std::shared_ptr<ExpressionAST> condition,
			  const std::vector<std::shared_ptr<StatementAST>> & body)
		: Condition(condition),
		  Body(body)
	{
	}
};

class PrintStatementAST : public StatementAST {
	std::vector<std::shared_ptr<ExpressionAST>> Arguments;

public:
	PrintStatementAST(const std::vector<std::shared_ptr<ExpressionAST>> & arguments)
		: Arguments(arguments)
	{
	}
};

class ExpressionStatementAST : public StatementAST {
	std::shared_ptr<ExpressionAST> Expression;

public:
	ExpressionStatementAST(std::shared_ptr<ExpressionAST> expression)
		: Expression(expression)
	{
	}
};

class FunctionDefinitionAST : public StatementAST {
	std::string Name;
	std::vector<std::string> Parameters;
	std::vector<std::shared_ptr<StatementAST>> Statements;

public:
	FunctionDefinitionAST(const std::string & name,
			      const std::vector<std::string> & parameters,
			      const std::vector<std::shared_ptr<StatementAST>> & statements)
		: Name(name), Parameters(parameters), Statements(statements)
	{
	}
};


class BinaryExpressionAST : public ExpressionAST {
public:
	enum BinaryOp {
		Add,
		Subtract,
		Multiply,
		Divide,
		Modulo,
		Exponentiate,
		And,
		Or,
	};

private:
	enum BinaryOp Op;
	std::shared_ptr<ExpressionAST> LHS, RHS;

public:

	BinaryExpressionAST(enum BinaryOp op,
			    std::shared_ptr<ExpressionAST> lhs,
			    std::shared_ptr<ExpressionAST> rhs)
		: Op(op), LHS(lhs), RHS(rhs)
	{
	}
};

class NumberExpressionAST : public ExpressionAST {
	int32_t Number;
public:
	NumberExpressionAST(int32_t number) : Number (number) {
	}
};

class CallExpressionAST : public ExpressionAST {
	std::string Callee;
	std::vector<std::shared_ptr<ExpressionAST>> Args;

public:
	CallExpressionAST(const std::string &callee,
			  const std::vector<std::shared_ptr<ExpressionAST>> &args)
		: Callee(callee), Args(args)
	{
	}
};

class Parser {
private:
	Lexer Lexer;
	static const char * const Tag;
	std::unique_ptr<Token> CurrentToken;

	std::unique_ptr<ExpressionAST>          parseNumberExpression();
	std::unique_ptr<ExpressionAST>          parseParenthesizedExpression();
	std::unique_ptr<ExpressionAST>          parseExpression();
	std::unique_ptr<FunctionDefinitionAST>  parseFunctionDefinition();
	std::unique_ptr<IfStatementAST>         parseIfStatement();
	std::unique_ptr<PassStatementAST>       parsePassStatement();
	std::unique_ptr<PrintStatementAST>      parsePrintStatement();
	std::unique_ptr<ReturnStatementAST>     parseReturnStatement();
	std::unique_ptr<WhileStatementAST>      parseWhileStatement();
	std::unique_ptr<ExpressionStatementAST> parseExpressionStatement();
	std::unique_ptr<StatementAST>           parseStatement();

	void parseError(const char *format, ...);

	void nextToken()
	{
		CurrentToken = Lexer.getNextToken();
	}
public:
	explicit Parser(std::shared_ptr<llvm::MemoryBuffer> buffer) :
		Lexer(buffer)
	{
		nextToken();
	}

	~Parser() { }

	std::unique_ptr<AST> buildAST();
};

} // End garter namespace
