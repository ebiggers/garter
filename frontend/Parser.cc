#include "Parser.h"
#include <stdio.h>
#include <stdarg.h>

using namespace garter;

const char *BinaryExpressionAST::getOpStr() const
{
	switch (Op) {
	case BinaryExpressionAST::None:
		break;
	case BinaryExpressionAST::Or:
		return "Or";
	case BinaryExpressionAST::And:
		return "And";
	case BinaryExpressionAST::LessThan:
		return "LessThan";
	case BinaryExpressionAST::GreaterThan:
		return "GreaterThan";
	case BinaryExpressionAST::LessThanOrEqualTo:
		return "LessThanOrEqualTo";
	case BinaryExpressionAST::GreaterThanOrEqualTo:
		return "GreaterThanOrEqualTo";
	case BinaryExpressionAST::EqualTo:
		return "EqualTo";
	case BinaryExpressionAST::NotEqualTo:
		return "NotEqualTo";
	case BinaryExpressionAST::In:
		return "In";
	case BinaryExpressionAST::NotIn:
		return "NotIn";
	case BinaryExpressionAST::Add:
		return "Add";
	case BinaryExpressionAST::Subtract:
		return "Subtract";
	case BinaryExpressionAST::Multiply:
		return "Multiply";
	case BinaryExpressionAST::Divide:
		return "Divide";
	case BinaryExpressionAST::Modulo:
		return "Modulo";
	case BinaryExpressionAST::Exponentiate:
		return "Exponentiate";
	}
	return "???";
}

const char *UnaryExpressionAST::getOpStr() const
{
	switch (Op) {
	case UnaryExpressionAST::Not:
		return "Not";
	case UnaryExpressionAST::Plus:
		return "Plus";
	case UnaryExpressionAST::Minus:
		return "Minus";
	}
	return "???";
}

void ProgramAST::print(std::ostream & os) const
{
	os << "Program {";
	os << "TopLevelItems = [";
	for (auto itemptr : TopLevelItems)
		os << *itemptr << ",";

	os << "]";
	os << "}";
}

void FunctionDefinitionAST::print(std::ostream & os) const
{
	os << "FunctionDefinition {";
	os << "Name = \"" << Name << "\",";
	os << "Parameters = [";
	for (const std::string & param  : Parameters)
		os << '"' << param << '"' << ",";

	os << "], Body = [";
	for (auto stmtptr : Body)
		os << *stmtptr << ",";
	os << "]";
	os << "}";
}

void VariableExpressionAST::print(std::ostream & os) const
{
	os << "VariableExpression {";
	os << "Name = \"" << Name << "\",";
	os << "}";
}

void AssignmentStatementAST::print(std::ostream & os) const
{
	os << "AssignmentStatement {";
	os << "Variable = " << *Variable << ",";
	os << "Expression = " << *Expression << ",";
	os << "}";
}

void PassStatementAST::print(std::ostream & os) const
{
	os << "PassStatement";
}

void ReturnStatementAST::print(std::ostream & os) const
{
	os << "ReturnStatement {";
	os << "Expression = " << *Expression;
	os << "}";
}

void IfStatementAST::print(std::ostream & os) const
{
	os << "IfStatement {";
	os << "Condition = " << *Condition << ",";
	os << "Statements = [";
	for (auto stmtptr : Body)
		os << *stmtptr << ",";
	os << "],";
	os << "ElifClauses = [";
	for (auto clauseptr : ElifClauses) {
		clauseptr->print(os);
		os << ",";
	}
	os << "],";
	os << "ElseStatements = [";
	for (auto stmtptr : ElseBody)
		os << *stmtptr << ",";
	os << "]";
	os << "}";
}

void IfStatementAST::ElifClause::print(std::ostream & os) const
{
	os << "ElifClause {";
	os << "Condition = " << *Condition << ",";
	os << "Statements = [";
	for (auto stmtptr : Body)
		os << *stmtptr << ",";
	os << "]";
	os << "}";
}

void WhileStatementAST::print(std::ostream & os) const
{
	os << "WhileStatement {";
	os << "Condition = " << *Condition << ",";
	os << "Body = [";
	for (auto stmtptr : Body)
		os << *stmtptr << ",";
	os << "]";
	os << "}";
}

void PrintStatementAST::print(std::ostream & os) const
{
	os << "PrintStatement {";
	os << "Arguments = [";
	for (auto argptr : Arguments)
		os << *argptr << ",";
	os << "]";
	os << "}";
}

void ExpressionStatementAST::print(std::ostream & os) const
{
	os << "ExpressionStatement {";
	os << "Expression = " << *Expression;
	os << "}";
}

void BinaryExpressionAST::print(std::ostream & os) const
{
	os << "BinaryExpression {";
	os << "Op = \"" << getOpStr() << "\",";
	os << "LHS = " << *LHS << ",";
	os << "RHS = " << *RHS << ",";
	os << "}";
}


void UnaryExpressionAST::print(std::ostream & os) const
{
	os << "UnaryExpression {";
	os << "Op = \"" << getOpStr() << "\",";
	os << "Expression = " << *Expression;
	os << "}";
}

void NumberExpressionAST::print(std::ostream & os) const
{
	os << "NumberExpression {";
	os << "Number = " << Number;
	os << "}";
}

void CallExpressionAST::print(std::ostream & os) const
{
	os << "CallExpression {";
	os << "Callee = \"" << Callee << "\",";
	os << "Arguments = [";
	for (auto argptr : Arguments)
		os << *argptr << ",";
	os << "]";
	os << "}";
}

BinaryExpressionAST::BinaryOp
Parser::currentMultiplicationOperator()
{
	switch (CurrentToken->getType()) {
	case Token::Asterisk:
		return BinaryExpressionAST::Multiply;
	case Token::ForwardSlash:
		return BinaryExpressionAST::Divide;
	case Token::Percent:
		return BinaryExpressionAST::Modulo;
	default:
		return BinaryExpressionAST::None;
	}
}

BinaryExpressionAST::BinaryOp
Parser::currentAdditionOperator()
{
	switch (CurrentToken->getType()) {
	case Token::Plus:
		return BinaryExpressionAST::Add;
	case Token::Minus:
		return BinaryExpressionAST::Subtract;
	default:
		return BinaryExpressionAST::None;
	}
}

BinaryExpressionAST::BinaryOp
Parser::currentComparisonOperator()
{
	switch (CurrentToken->getType()) {
	case Token::LessThanOrEqualTo:
		return BinaryExpressionAST::LessThanOrEqualTo;
	case Token::GreaterThanOrEqualTo:
		return BinaryExpressionAST::GreaterThanOrEqualTo;
	case Token::GreaterThan:
		return BinaryExpressionAST::GreaterThan;
	case Token::LessThan:
		return BinaryExpressionAST::LessThan;
	case Token::DoubleEquals:
		return BinaryExpressionAST::EqualTo;
	case Token::NotEqualTo:
		return BinaryExpressionAST::NotEqualTo;
	case Token::In:
		return BinaryExpressionAST::In;
	case Token::Not:
		nextTokenLookahead();
		if (NextToken->getType() == Token::In)
			return BinaryExpressionAST::NotIn;
		else
			return BinaryExpressionAST::None;
	default:
		return BinaryExpressionAST::None;
	}
}

/*
 * <num_expr> ::=
 *	number
 */
std::unique_ptr<ExpressionAST>
Parser::parseNumberExpression()
{
	assert(CurrentToken->getType() == Token::Number);
	std::unique_ptr<ExpressionAST> num_expr(
			new NumberExpressionAST(CurrentToken->getNumber()));
	nextToken();
	return num_expr;
}

/*
 * <identifier_expr> ::=
 *	identifier |
 *	identifier \( (expr (, expr)*)? \)
 */
std::unique_ptr<ExpressionAST>
Parser::parseIdentifierExpression()
{
	assert(CurrentToken->getType() == Token::Identifier);
	std::string name(CurrentToken->getName());
	nextToken();
	std::vector<std::shared_ptr<ExpressionAST>> args;

	if (CurrentToken->getType() != Token::LeftParenthesis) {
		return std::unique_ptr<ExpressionAST>(
				new VariableExpressionAST(name));
	}
	nextToken();

	if (CurrentToken->getType() != Token::RightParenthesis) {
		for (;;) {
			std::unique_ptr<ExpressionAST> expr;

			expr = parseExpression();
			if (expr == nullptr)
				return nullptr;

			args.push_back(std::move(expr));

			if (CurrentToken->getType() == Token::Comma) {
				nextToken();
			} else if (CurrentToken->getType() == Token::RightParenthesis) {
				break;
			} else {
				Lexer.reportError("expected ',' or ')' in function call");
				return nullptr;
			}
		}
	}
	nextToken();

	return std::unique_ptr<ExpressionAST>(new CallExpressionAST(name, args));
}

/*
 * <paren_expr> ::=
 *	( <expr> )
 */
std::unique_ptr<ExpressionAST>
Parser::parseParenthesizedExpression()
{
	assert(CurrentToken->getType() == Token::LeftParenthesis);
	nextToken();

	std::unique_ptr<ExpressionAST> expression = parseExpression();

	if (expression == nullptr)
		return nullptr;

	if (CurrentToken->getType() != Token::RightParenthesis) {
		Lexer.reportError("expected ')'");
		return nullptr;
	}
	nextToken();

	return expression;
}

/*
 * <primary_expr> ::=
 *	  <identifier_expr>
 *	| <num_expr>
 *	| ( <expr> )
 */
std::unique_ptr<ExpressionAST>
Parser::parsePrimaryExpression()
{
	switch (CurrentToken->getType()) {
	case Token::Identifier:
		return parseIdentifierExpression();
	case Token::Number:
		return parseNumberExpression();
	case Token::LeftParenthesis:
		return parseParenthesizedExpression();
	case Token::EndOfFile:
		Lexer.reportError("unexpected end of file");
		return nullptr;
	case Token::Error:
		return nullptr;
	default:
		Lexer.reportError("expected start of primary expression");
		return nullptr;
	}
}

/*
 * <power_expr> ::=
 *	  <primary_expr>
 *	| <primary_expr> ** <unary_expr>
 */
std::unique_ptr<ExpressionAST>
Parser::parsePowerExpression()
{
	std::unique_ptr<ExpressionAST> primary_expr;

	primary_expr = parsePrimaryExpression();
	if (primary_expr == nullptr)
		return nullptr;

	if (CurrentToken->getType() != Token::DoubleAsterisk)
		return primary_expr;

	nextToken();
	std::unique_ptr<ExpressionAST> unary_expr = parseUnaryExpression();
	if (unary_expr == nullptr)
		return nullptr;

	return std::unique_ptr<ExpressionAST>(
			new BinaryExpressionAST(BinaryExpressionAST::Exponentiate,
						std::move(primary_expr),
						std::move(unary_expr)));
}

/*
 * <unary_expr> ::=
 *	  <power_expr>
 *	| - <unary_expr>
 *	| + <unary_expr>
 */
std::unique_ptr<ExpressionAST>
Parser::parseUnaryExpression()
{
	UnaryExpressionAST::UnaryOp op;

	switch (CurrentToken->getType()) {
	case Token::Minus:
		op = UnaryExpressionAST::Minus;
		break;
	case Token::Plus:
		op = UnaryExpressionAST::Plus;
		break;
	default:
		return parsePowerExpression();
	}

	nextToken();

	std::unique_ptr<ExpressionAST> unary_expr = parseUnaryExpression();
	if (unary_expr == nullptr)
		return nullptr;

	return std::unique_ptr<ExpressionAST>(
			new UnaryExpressionAST(op, std::move(unary_expr)));
}

/*
 * <mult_expr> ::=
 *	<unary_expr> (<mult_op> <unary_expr>)*
 *
 * <mult_op> ::=
 *	  *
 *	| /
 *	| %
 */
std::unique_ptr<ExpressionAST>
Parser::parseMultiplicationExpression()
{
	std::unique_ptr<ExpressionAST> mult_expr;
	BinaryExpressionAST::BinaryOp op;

	mult_expr = parseUnaryExpression();
	if (mult_expr == nullptr)
		return nullptr;

	while ((op = currentMultiplicationOperator()) != BinaryExpressionAST::None) {
		nextToken();

		std::unique_ptr<ExpressionAST> unary_expr = parseUnaryExpression();
		if (unary_expr == nullptr)
			return nullptr;
		mult_expr = std::unique_ptr<ExpressionAST>(
			new BinaryExpressionAST(op,
						std::move(mult_expr),
						std::move(unary_expr)));
	}
	return mult_expr;
}

/*
 * <add_expr> ::=
 *	<mult_expr> (<add_op> <mult_expr>)*
 *
 * <add_op> ::=
 *	+ | -
 */
std::unique_ptr<ExpressionAST>
Parser::parseAdditionExpression()
{
	std::unique_ptr<ExpressionAST> add_expr;
	BinaryExpressionAST::BinaryOp op;

	add_expr = parseMultiplicationExpression();
	if (add_expr == nullptr)
		return nullptr;

	while ((op = currentAdditionOperator()) != BinaryExpressionAST::None) {
		nextToken();

		std::unique_ptr<ExpressionAST> mult_expr = parseMultiplicationExpression();
		if (mult_expr == nullptr)
			return nullptr;
		add_expr = std::unique_ptr<ExpressionAST>(
			new BinaryExpressionAST(op,
						std::move(add_expr),
						std::move(mult_expr)));
	}
	return add_expr;
}

/*
 * <comp_expr> ::=
 *	<add_expr> (<comp_op> <add_expr>)*
 *
 * <comp_op> ::=
 *	  <
 *	| >
 *	| <=
 *	| >=
 *	| ==
 *	| !=
 *	| in
 *	| not in
 */
std::unique_ptr<ExpressionAST>
Parser::parseComparisonExpression()
{
	std::unique_ptr<ExpressionAST> comp_expr;
	BinaryExpressionAST::BinaryOp op;

	comp_expr = parseAdditionExpression();
	if (comp_expr == nullptr)
		return nullptr;

	while ((op = currentComparisonOperator()) != BinaryExpressionAST::None) {
		nextToken();
		if (op == BinaryExpressionAST::NotIn)
			nextToken();

		std::unique_ptr<ExpressionAST> add_expr = parseAdditionExpression();
		if (add_expr == nullptr)
			return nullptr;
		comp_expr = std::unique_ptr<ExpressionAST>(
			new BinaryExpressionAST(op,
						std::move(comp_expr),
						std::move(add_expr)));
	}
	return comp_expr;
}

/*
 * <not_expr> ::=
 *	(not)* <comp_expr>
 */
std::unique_ptr<ExpressionAST>
Parser::parseNotExpression()
{
	unsigned long num_nots = 0;
	std::unique_ptr<ExpressionAST> comparison_expression;
	std::unique_ptr<ExpressionAST> not_expression;

	while (CurrentToken->getType() == Token::Not)
	{
		num_nots++;
		nextToken();
	}

	comparison_expression = parseComparisonExpression();
	if (comparison_expression == nullptr)
		return nullptr;

	if (num_nots == 0)
		return comparison_expression;

	not_expression = std::move(comparison_expression);
	do {
		not_expression = std::unique_ptr<ExpressionAST>(
			new UnaryExpressionAST(UnaryExpressionAST::Not,
					       std::move(not_expression)));
	} while (--num_nots);

	return not_expression;
}

/*
 * <and_expr> ::=
 *	<not_expr> (and <not_expr>)*
 */
std::unique_ptr<ExpressionAST>
Parser::parseAndExpression()
{
	std::unique_ptr<ExpressionAST> and_expr = parseNotExpression();

	if (and_expr == nullptr)
		return nullptr;

	while (CurrentToken->getType() == Token::And) {
		nextToken();
		std::unique_ptr<ExpressionAST> not_expr = parseNotExpression();
		if (not_expr == nullptr)
			return nullptr;
		and_expr = std::unique_ptr<ExpressionAST>(
			new BinaryExpressionAST(BinaryExpressionAST::And,
						std::move(and_expr),
						std::move(not_expr)));
	}

	return and_expr;
}

/*
 * <expr> ::=
 *	<and_expr> (or <and_expr>)*
 */
std::unique_ptr<ExpressionAST>
Parser::parseExpression()
{
	std::unique_ptr<ExpressionAST> or_expr = parseAndExpression();
	if (or_expr == nullptr)
		return nullptr;

	while (CurrentToken->getType() == Token::Or) {
		nextToken();

		std::unique_ptr<ExpressionAST> and_expr = parseAndExpression();
		if (and_expr == nullptr)
			return nullptr;
		or_expr = std::unique_ptr<ExpressionAST>(
			new BinaryExpressionAST(BinaryExpressionAST::Or,
						std::move(or_expr),
						std::move(and_expr)));
	}
	return or_expr;
}

/* <funcdef> ::=
 *	(extern)? def <identifier> \( (identifier (, identifier)*)? \) : <stmt>+  enddef
 */
std::unique_ptr<FunctionDefinitionAST>
Parser::parseFunctionDefinition()
{
	std::string name;
	std::vector<std::string> parameters;
	std::vector<std::shared_ptr<StatementAST>> statements;
	bool is_extern;

	if (CurrentToken->getType() == Token::Extern) {
		is_extern = true;
		nextToken();
		if (CurrentToken->getType() != Token::Def) {
			Lexer.reportError("expected 'def' after 'extern'");
			return nullptr;
		}
	} else {
		is_extern = false;
		assert(CurrentToken->getType() == Token::Def);
	}

	nextToken();

	if (CurrentToken->getType() != Token::Identifier) {
		Lexer.reportError("expected identifier (function name) after 'def'");
		return nullptr;
	}
	name = CurrentToken->getName();
	nextToken();

	if (CurrentToken->getType() != Token::LeftParenthesis) {
		Lexer.reportError("expected '('");
		return nullptr;
	}
	nextToken();

	if (CurrentToken->getType() != Token::RightParenthesis) {
		for (;;) {
			if (CurrentToken->getType() != Token::Identifier) {
				Lexer.reportError("expected identifier (named parameter) "
						  "in function prototype");
				return nullptr;
			}

			parameters.push_back(std::string(CurrentToken->getName()));
			nextToken();

			if (CurrentToken->getType() == Token::Comma) {
				nextToken();
			} else if (CurrentToken->getType() == Token::RightParenthesis) {
				break;
			} else {
				Lexer.reportError("expected ',' or ')' in function prototype");
				return nullptr;
			}
		}
	}
	nextToken();

	if (CurrentToken->getType() != Token::Colon) {
		Lexer.reportError("expected ':'");
		return nullptr;
	}
	nextToken();

	do {
		std::unique_ptr<StatementAST> statement = parseStatement();

		if (statement == nullptr)
			return nullptr;
		statements.push_back(std::move(statement));
	} while (CurrentToken->getType() != Token::EndDef);
	nextToken();

	return std::unique_ptr<FunctionDefinitionAST>(
			new FunctionDefinitionAST(name, parameters, statements, is_extern));
}


/* <assignment_stmt> ::=
 *	<identifier> = <expr> ;
 */
std::unique_ptr<AssignmentStatementAST>
Parser::parseAssignmentStatement()
{
	assert(CurrentToken->getType() == Token::Identifier);
	std::unique_ptr<VariableExpressionAST> lhs(
			new VariableExpressionAST(CurrentToken->getName()));
	nextToken();

	assert(CurrentToken->getType() == Token::Equals);
	nextToken();

	std::unique_ptr<ExpressionAST> rhs = parseExpression();
	if (rhs == nullptr)
		return nullptr;

	if (CurrentToken->getType() != Token::Semicolon) {
		Lexer.reportError("expected ';'");
		return nullptr;
	}
	nextToken();

	return std::unique_ptr<AssignmentStatementAST>(
			new AssignmentStatementAST(std::move(lhs),
						   std::move(rhs)));
}

/* <expr_stmt> ::=
 *	<expr> ;
 */
std::unique_ptr<ExpressionStatementAST>
Parser::parseExpressionStatement()
{
	std::unique_ptr<ExpressionAST> expression = parseExpression();

	if (expression == nullptr)
		return nullptr;

	if (CurrentToken->getType() != Token::Semicolon) {
		Lexer.reportError("expected ';'");
		return nullptr;
	}

	nextToken();

	return std::unique_ptr<ExpressionStatementAST>(
			new ExpressionStatementAST(std::move(expression)));
}

/* <if_stmt> ::=
 *	if <expr> : <stmt>+ (elif <expr>: <stmt>+)* (else: <stmt>+)? endif
 */
std::unique_ptr<IfStatementAST>
Parser::parseIfStatement()
{
	std::unique_ptr<ExpressionAST> condition;
	std::vector<std::shared_ptr<StatementAST>> statements;
	std::vector<std::shared_ptr<IfStatementAST::ElifClause>> elif_clauses;
	std::vector<std::shared_ptr<StatementAST>> else_statements;

	assert(CurrentToken->getType() == Token::If);
	nextToken();

	condition = parseExpression();
	if (condition == nullptr)
		return nullptr;

	if (CurrentToken->getType() != Token::Colon) {
		Lexer.reportError("expected ':'");
		return nullptr;
	}
	nextToken();

	do {
		std::unique_ptr<StatementAST> statement = parseStatement();

		if (statement == nullptr)
			return nullptr;
		statements.push_back(std::move(statement));
	} while (CurrentToken->getType() != Token::EndIf &&
		 CurrentToken->getType() != Token::Else &&
		 CurrentToken->getType() != Token::Elif);

	while (CurrentToken->getType() == Token::Elif)
	{
		std::unique_ptr<ExpressionAST> elif_condition;
		std::vector<std::shared_ptr<StatementAST>> elif_statements;

		nextToken();

		elif_condition = parseExpression();
		if (elif_condition == nullptr)
			return nullptr;

		if (CurrentToken->getType() != Token::Colon) {
			Lexer.reportError("expected ':'");
			return nullptr;
		}

		nextToken();

		do {
			std::unique_ptr<StatementAST> statement = parseStatement();

			if (statement == nullptr)
				return nullptr;
			elif_statements.push_back(std::move(statement));
		} while (CurrentToken->getType() != Token::EndIf &&
			 CurrentToken->getType() != Token::Else &&
			 CurrentToken->getType() != Token::Elif);
		elif_clauses.push_back(
			std::shared_ptr<IfStatementAST::ElifClause>(
				new IfStatementAST::ElifClause(
					std::move(elif_condition),
						elif_statements)));
	}

	if (CurrentToken->getType() == Token::Else) {

		nextToken();

		if (CurrentToken->getType() != Token::Colon) {
			Lexer.reportError("expected ':'");
			return nullptr;
		}

		nextToken();

		do {
			std::unique_ptr<StatementAST> statement = parseStatement();
			if (statement == nullptr)
				return nullptr;
			else_statements.push_back(std::move(statement));
		} while (CurrentToken->getType() != Token::EndIf);
	}

	nextToken();

	return std::unique_ptr<IfStatementAST>(
			new IfStatementAST(std::move(condition), statements,
					   elif_clauses, else_statements));
}

/* <pass_stmt> ::=
 *	pass ;
 */
std::unique_ptr<PassStatementAST>
Parser::parsePassStatement()
{
	assert(CurrentToken->getType() == Token::Pass);

	nextToken();

	if (CurrentToken->getType() != Token::Semicolon) {
		Lexer.reportError("expected ';'");
		return nullptr;
	}

	nextToken();

	return std::unique_ptr<PassStatementAST>(new PassStatementAST());
}

/* <print_stmt> ::=
 *	print (<expr> ,)* ;
 */
std::unique_ptr<PrintStatementAST>
Parser::parsePrintStatement()
{
	std::vector<std::shared_ptr<ExpressionAST>> expressions;

	assert(CurrentToken->getType() == Token::Print);

	nextToken();

	if (CurrentToken->getType() != Token::Semicolon) {
		for (;;) {
			std::unique_ptr<ExpressionAST> expression = parseExpression();

			if (expression == nullptr)
				return nullptr;

			expressions.push_back(std::move(expression));

			if (CurrentToken->getType() == Token::Comma) {
				nextToken();
			} else if (CurrentToken->getType() == Token::Semicolon) {
				break;
			} else {
				Lexer.reportError("expected ';' or ','");
				return nullptr;
			}
		}
	}

	nextToken();

	return std::unique_ptr<PrintStatementAST>(
			new PrintStatementAST(expressions));
}

/* <return_stmt> ::=
 *	return <expr> ;
 */
std::unique_ptr<ReturnStatementAST>
Parser::parseReturnStatement()
{
	assert(CurrentToken->getType() == Token::Return);

	nextToken();

	std::unique_ptr<ExpressionAST> expression = parseExpression();

	if (expression == nullptr)
		return nullptr;

	if (CurrentToken->getType() != Token::Semicolon) {
		Lexer.reportError("expected ';'");
		return nullptr;
	}

	nextToken();

	return std::unique_ptr<ReturnStatementAST>(
				new ReturnStatementAST(std::move(expression)));
}

/* <while_stmt> ::=
 *	while <expr> : <stmt>+ endwhile
 */
std::unique_ptr<WhileStatementAST>
Parser::parseWhileStatement()
{
	assert(CurrentToken->getType() == Token::While);

	nextToken();

	std::unique_ptr<ExpressionAST> condition = parseExpression();

	if (CurrentToken->getType() != Token::Colon) {
		Lexer.reportError("expected ':'");
		return nullptr;
	}

	nextToken();

	std::vector<std::shared_ptr<StatementAST>> body;

	do {
		std::unique_ptr<StatementAST> statement = parseStatement();
		if (statement == nullptr)
			return nullptr;
		body.push_back(std::move(statement));
	} while (CurrentToken->getType() != Token::EndWhile);

	nextToken();

	return std::unique_ptr<WhileStatementAST>(
				new WhileStatementAST(std::move(condition), body));
}

/* <stmt> ::=
 *	<assignment_stmt>
 *	| <expr_stmt>
 *	| <if_stmt>
 *	| <pass_stmt>
 *	| <print_stmt>
 *	| <return_stmt>
 *	| <while_stmt>
 */
std::unique_ptr<StatementAST>
Parser::parseStatement()
{
	switch (CurrentToken->getType()) {
	case Token::Identifier:
		nextTokenLookahead();
		if (NextToken->getType() == Token::Equals)
			return parseAssignmentStatement();
		else
			return parseExpressionStatement();
	case Token::If:
		return parseIfStatement();
	case Token::Pass:
		return parsePassStatement();
	case Token::Print:
		return parsePrintStatement();
	case Token::Return:
		return parseReturnStatement();
	case Token::While:
		return parseWhileStatement();
	default:
		return parseExpressionStatement();
	}
}

/* <toplevel_item> ::=
 *	<stmt>
 *	| <funcdef>
 */
std::unique_ptr<ASTBase>
Parser::parseTopLevelItem()
{
	switch (CurrentToken->getType()) {
	case Token::Error:
	case Token::EndOfFile:
		return nullptr;
	case Token::Def:
	case Token::Extern:
		return parseFunctionDefinition();
	default:
		return parseStatement();
	}
}

/* <program> ::=
 *	<toplevel_item>*
 */
std::unique_ptr<ProgramAST>
Parser::parseProgram()
{
	std::vector<std::shared_ptr<ASTBase>> top_level_items;

	for (;;) {
		std::unique_ptr<ASTBase> ast = parseTopLevelItem();

		if (ast != nullptr)
			top_level_items.push_back(std::move(ast));
		else if (reachedEndOfFile())
			break;
		else
			return nullptr;
	}
	return std::unique_ptr<ProgramAST>(new ProgramAST(top_level_items));
}
