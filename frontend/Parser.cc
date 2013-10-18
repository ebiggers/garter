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

std::unique_ptr<ExpressionAST>
Parser::parseNumberExpression()
{
	assert(CurrentToken->getType() == Token::Number);
	std::unique_ptr<ExpressionAST> ast(
			new NumberExpressionAST(CurrentToken->getNumber()));
	nextToken();
	return ast;
}

std::unique_ptr<ExpressionAST>
Parser::parseExpression()
{
	/* TODO */
	return nullptr;
}

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


std::unique_ptr<FunctionDefinitionAST>
Parser::parseFunctionDefinition()
{
	assert(CurrentToken->getType() == Token::Def);

	nextToken();

	/* TODO */

	return nullptr;
}

std::unique_ptr<IfStatementAST>
Parser::parseIfStatement()
{
	assert(CurrentToken->getType() == Token::If);

	nextToken();

	return nullptr;
}

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

std::unique_ptr<PrintStatementAST>
Parser::parsePrintStatement()
{
	assert(CurrentToken->getType() == Token::Print);

	nextToken();

	std::vector<std::shared_ptr<ExpressionAST>> expressions;

	while (CurrentToken->getType() != Token::Semicolon)
	{
		std::unique_ptr<ExpressionAST> expression = parseExpression();

		if (expression == nullptr)
			return nullptr;

		expressions.push_back(std::move(expression));
	}

	nextToken();

	return std::unique_ptr<PrintStatementAST>(
			new PrintStatementAST(expressions));
}

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

	std::vector<std::shared_ptr<StatementAST>> body;

	while (CurrentToken->getType() != Token::EndWhile)
	{
		std::unique_ptr<StatementAST> statement = parseStatement();
		if (statement == nullptr)
			return nullptr;
		body.push_back(std::move(statement));
	}

	nextToken();

	return std::unique_ptr<WhileStatementAST>(
				new WhileStatementAST(std::move(condition), body));
}

std::unique_ptr<ExpressionStatementAST>
Parser::parseExpressionStatement()
{
	std::unique_ptr<ExpressionAST> expression = parseExpression();

	if (expression == nullptr)
		return nullptr;

	if (CurrentToken->getType() != Token::Semicolon)
	{
		parseError("Expected ';'\n");
		return nullptr;
	}
	return std::unique_ptr<ExpressionStatementAST>(
			new ExpressionStatementAST(std::move(expression)));
}

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

std::unique_ptr<AST>
Parser::buildAST()
{
	std::vector<std::shared_ptr<AST>> top_level_items;

	nextToken();
	
	for (;;) {
		std::unique_ptr<AST> ast;
		switch (CurrentToken->getType()) {
		case Token::EndOfFile:
			break;
		case Token::Error:
			break;
		case Token::Def:
			ast = parseFunctionDefinition();
			break;
		default:
			ast = parseStatement();
			break;
		}
		if (!ast)
			return nullptr;
		top_level_items.push_back(std::move(ast));
	}

	return std::unique_ptr<AST>(new ProgramAST(top_level_items));
}
