#include <frontend/Lexer.h>
#include <memory>
#include <vector>
#include <ostream>

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

class ProgramAST : public ASTBase {
	std::vector<std::shared_ptr<ASTBase>> TopLevelItems;
public:
	ProgramAST(const std::vector<std::shared_ptr<ASTBase>> &top_level_items)
		: TopLevelItems(top_level_items)
	{
	}

	void print(std::ostream & os) const;
};

class StatementAST : public ASTBase {
};


class FunctionDefinitionAST : public ASTBase {
	std::string Name;
	std::vector<std::string> Parameters;
	std::vector<std::shared_ptr<StatementAST>> Body;

public:
	FunctionDefinitionAST(const std::string & name,
			      const std::vector<std::string> & parameters,
			      const std::vector<std::shared_ptr<StatementAST>> & body)
		: Name(name), Parameters(parameters), Body(body)
	{
	}

	void print(std::ostream & os) const;
};

class ExpressionAST : public ASTBase {
};

class VariableExpressionAST : public ExpressionAST {
	std::string Name;
public:
	VariableExpressionAST(const std::string &name)
		: Name(name)
	{
	}

	void print(std::ostream & os) const;
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

	void print(std::ostream & os) const;
};

class PassStatementAST : public StatementAST {
public:
	void print(std::ostream & os) const;
};

class ReturnStatementAST : public StatementAST {
	std::shared_ptr<ExpressionAST> Expression;
public:
	ReturnStatementAST(std::shared_ptr<ExpressionAST> expression)
		: Expression(expression)
	{
	}

	void print(std::ostream & os) const;
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

		void print(std::ostream & os) const;
	};

private:
	std::shared_ptr<ExpressionAST> Condition;
	std::vector<std::shared_ptr<StatementAST>> Body;
	std::vector<std::shared_ptr<ElifClause>> ElifClauses;
	std::vector<std::shared_ptr<StatementAST>> ElseBody;

public:
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
	void print(std::ostream & os) const;
};

class PrintStatementAST : public StatementAST {
	std::vector<std::shared_ptr<ExpressionAST>> Arguments;

public:
	PrintStatementAST(const std::vector<std::shared_ptr<ExpressionAST>> & arguments)
		: Arguments(arguments)
	{
	}
	void print(std::ostream & os) const;
};

class ExpressionStatementAST : public StatementAST {
	std::shared_ptr<ExpressionAST> Expression;

public:
	ExpressionStatementAST(std::shared_ptr<ExpressionAST> expression)
		: Expression(expression)
	{
	}
	void print(std::ostream & os) const;
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
		In,
		NotIn,
		Add,
		Subtract,
		Multiply,
		Divide,
		Modulo,
		Exponentiate,
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

	const char *getOpStr() const;
	void print(std::ostream & os) const;
};

class UnaryExpressionAST : public ExpressionAST {
public:
	enum UnaryOp {
		Not,
		Minus,
		Plus,
	};

private:
	enum UnaryOp Op;
	std::shared_ptr<ExpressionAST> Expression;

public:

	UnaryExpressionAST(enum UnaryOp op,
			   std::shared_ptr<ExpressionAST> expression)
		: Op(op), Expression(expression)
	{
	}
	const char *getOpStr() const;
	void print(std::ostream & os) const;
};

class NumberExpressionAST : public ExpressionAST {
	int32_t Number;
public:
	NumberExpressionAST(int32_t number) : Number (number) {
	}
	void print(std::ostream & os) const;
};

class CallExpressionAST : public ExpressionAST {
	std::string Callee;
	std::vector<std::shared_ptr<ExpressionAST>> Arguments;

public:
	CallExpressionAST(const std::string &callee,
			  const std::vector<std::shared_ptr<ExpressionAST>> &arguments)
		: Callee(callee), Arguments(arguments)
	{
	}
	void print(std::ostream & os) const;
};

class Parser {
private:
	Lexer Lexer;
	static const char * const Tag;
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

	void parseError(const char *format, ...);

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
	explicit Parser(std::shared_ptr<llvm::MemoryBuffer> buffer) :
		Lexer(buffer)
	{
		nextToken();
	}

	~Parser() { }

	std::unique_ptr<ProgramAST> buildAST();
};

} // End garter namespace
