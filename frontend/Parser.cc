#include "Parser.h"
#include <stdio.h>
#include <stdarg.h>

using namespace garter;

const char * const Parser::Tag = "Parser";

void Parser::parseError(const char *format, ...)
{
	va_list va;

	va_start(va, format);
	fprintf(stderr, "%s: ", Tag);
	vfprintf(stderr, format, va);
	va_end(va);
}

/* <num_expr> ::=
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

/* <paren_expr> ::=
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
		parseError("Expected ')'\n");
		return nullptr;
	}
	nextToken();

	return expression;
}

std::unique_ptr<ExpressionAST>
Parser::parseExpression()
{
	/* TODO */
	return nullptr;
}

/* <funcdef> ::=
 *	def <identifier> \( (identifier (, identifier)*)? \) : <stmt>+  enddef
 */
std::unique_ptr<FunctionDefinitionAST>
Parser::parseFunctionDefinition()
{
	std::string name;
	std::vector<std::string> parameters;
	std::vector<std::shared_ptr<StatementAST>> statements;

	assert(CurrentToken->getType() == Token::Def);
	nextToken();

	if (CurrentToken->getType() != Token::Identifier) {
		parseError("Expected identifier (function name) after 'def'\n");
		return nullptr;
	}
	name = CurrentToken->getName();
	nextToken();

	if (CurrentToken->getType() != Token::LeftParenthesis) {
		parseError("Expected '('\n");
		return nullptr;
	}
	nextToken();

	if (CurrentToken->getType() != Token::RightParenthesis) {
		for (;;) {
			if (CurrentToken->getType() != Token::Identifier) {
				parseError("Expected identifier (named parameter) "
					   "in function prototype\n");
				return nullptr;
			}

			parameters.push_back(std::string(CurrentToken->getName()));
			nextToken();

			if (CurrentToken->getType() == Token::Comma) {
				nextToken();
			} else if (CurrentToken->getType() == Token::RightParenthesis) {
				break;
			} else {
				parseError("Expected ',' or ')' in function prototype\n");
				return nullptr;
			}
		}
	}
	nextToken();

	if (CurrentToken->getType() != Token::Colon) {
		parseError("Expected ':'\n");
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
			new FunctionDefinitionAST(name, parameters, statements));
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
		parseError("Expected ':'\n");
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
			parseError("Expected ':'\n");
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
			parseError("Expected ':'\n");
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
		parseError("Expected ';'\n");
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
				parseError("Expected ';' or ','\n");
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
		parseError("Expected ';'\n");
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
		parseError("Expected ':'\n");
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
		parseError("Expected ';'\n");
		return nullptr;
	}

	nextToken();

	return std::unique_ptr<ExpressionStatementAST>(
			new ExpressionStatementAST(std::move(expression)));
}

/* <stmt> ::=
 *	<if_stmt>
 *	| <pass_stmt>
 *	| <print_stmt>
 *	| <return_stmt>
 *	| <while_stmt>
 *	| <expr_stmt>
 */
std::unique_ptr<StatementAST>
Parser::parseStatement()
{
	switch (CurrentToken->getType()) {
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

/* <program> ::=
 *	<toplevel_item>*
 *
 * <toplevel_item> ::=
 *	<stmt>
 *	| <funcdef>
 */
std::unique_ptr<AST>
Parser::buildAST()
{
	std::vector<std::shared_ptr<AST>> top_level_items;

	while (CurrentToken->getType() != Token::EndOfFile) {

		std::unique_ptr<AST> ast;

		switch (CurrentToken->getType()) {
		case Token::Error:
			ast = nullptr;
			break;
		case Token::Def:
			ast = parseFunctionDefinition();
			break;
		default:
			ast = parseStatement();
			break;
		}
		if (ast == nullptr)
			return nullptr;
		top_level_items.push_back(std::move(ast));
	}

	return std::unique_ptr<AST>(new ProgramAST(top_level_items));
}
