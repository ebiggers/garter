#include "Lexer.h"
#include <stdio.h>
#include <string.h>

using namespace garter;

const char * const Lexer::Tag = "Lexer";

enum CharType {
	LOWER_CASE = 0x01,
	UPPER_CASE = 0x02,
	UNDERSCORE = 0x04,
	NUMBER     = 0x08,
};

static const uint8_t CharTab[256] = {
	['a' ... 'z'] = LOWER_CASE,
	['A' ... 'Z'] = UPPER_CASE,
	['_']         = UNDERSCORE,
	['0' ... '9'] = NUMBER,
};

std::unique_ptr<Token> Lexer::lexIdentifierOrKeyword()
{
	const char *start;
	size_t len;

	start = NextCharPtr;

	do {
		NextCharPtr++;
	} while (CharTab[(uint8_t)*NextCharPtr] & (LOWER_CASE | UPPER_CASE |
						   UNDERSCORE | NUMBER));
	len = NextCharPtr - start;

	char *name = new char[len + 1];
	memcpy(name, start, len);
	name[len] = '\0';

	auto it_kw = Keywords.find(name);

	if (it_kw != Keywords.end()) {
		delete[] name;
		return std::unique_ptr<Token>(new Token(it_kw->second));
	} else {
		return std::unique_ptr<Token>(new Token(Token::Identifier, name));
	}
}

std::unique_ptr<Token> Lexer::lexNumber()
{
	int32_t n = 0;

	do {
		int32_t next_digit = (*NextCharPtr - '0');

		if (n > INT32_MAX / 10)
			goto too_large;

		n *= 10;

		if (n > INT32_MAX - next_digit)
			goto too_large;

		n += next_digit;

		NextCharPtr++;

	} while (*NextCharPtr >= '0' && *NextCharPtr <= '9');

	return std::unique_ptr<Token>(new Token(Token::Number, n));

too_large:
	fprintf(stderr, "%s: Integer constant on line %lu is "
		"too large!\n", Tag, CurrentLineNumber);
	return std::unique_ptr<Token>(new Token(Token::Error));
}

std::unique_ptr<Token> Lexer::getNextToken()
{
next_char:
	switch (*NextCharPtr) {
	case '\n':
		CurrentLineNumber++;
		// Fall through
	case ' ':
	case '\t':
	case '\v':
		// Skip whitespace character
		NextCharPtr++;
		goto next_char;

	case '#':
		// Skip comment
		do {
			NextCharPtr++;
		} while (*NextCharPtr != '\n' && *NextCharPtr != '\0');
		goto next_char;

	case 'a' ... 'z':
	case 'A' ... 'Z':
	case '_':
		return lexIdentifierOrKeyword();

	case '0' ... '9':
		return lexNumber();

	case '(':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::LeftParenthesis));

	case ')':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::RightParenthesis));

	case ':':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::Colon));

	case ';':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::Semicolon));

	case ',':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::Comma));

	case '[':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::LeftSquareBracket));

	case ']':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::RightSquareBracket));

	case '=':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			// Double equals (equality predicate)
			NextCharPtr++;
			return std::unique_ptr<Token>(new Token(Token::DoubleEquals));
		} else {
			// Equals (assignment)
			return std::unique_ptr<Token>(new Token(Token::Equals));
		}
		break;

	case '+':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::Plus));

	case '-':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::Minus));

	case '*':
		NextCharPtr++;
		if (*NextCharPtr == '*') {
			NextCharPtr++;
			return std::unique_ptr<Token>(new Token(Token::DoubleAsterisk));
		} else {
			return std::unique_ptr<Token>(new Token(Token::Asterisk));
		}
		break;

	case '/':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::ForwardSlash));

	case '%':
		NextCharPtr++;
		return std::unique_ptr<Token>(new Token(Token::Percent));

	case '<':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			NextCharPtr++;
			return std::unique_ptr<Token>(new Token(Token::LessThanOrEqualTo));
		} else {
			return std::unique_ptr<Token>(new Token(Token::LessThan));
		}

	case '>':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			NextCharPtr++;
			return std::unique_ptr<Token>(new Token(Token::GreaterThanOrEqualTo));
		} else {
			return std::unique_ptr<Token>(new Token(Token::GreaterThan));
		}

	case '!':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			// "Not equal to" symbol
			NextCharPtr++;
			return std::unique_ptr<Token>(new Token(Token::NotEqualTo));
		} else {
			// '!' followed by something else--- not valid
			fprintf(stderr, "%s: Unexpected character '%c' "
				"after '!' on line %lu\n",
				Tag, *NextCharPtr, CurrentLineNumber);
			return std::unique_ptr<Token>(new Token(Token::Error));
		}

	case '\0':
		// '\0' marks end of buffer
		return std::unique_ptr<Token>(new Token(Token::EndOfFile));

	default:
		fprintf(stderr, "%s: Unexpected character '%c' on line %lu\n",
			Tag, *NextCharPtr, CurrentLineNumber);
		return std::unique_ptr<Token>(new Token(Token::Error));
	}
}

Lexer::Lexer(const char *string) : NextCharPtr(string), CurrentLineNumber(1)
{
	Keywords.insert(std::make_pair("and", Token::And));
	Keywords.insert(std::make_pair("def", Token::Def));
	Keywords.insert(std::make_pair("else", Token::Else));
	Keywords.insert(std::make_pair("elif", Token::Elif));
	Keywords.insert(std::make_pair("enddef", Token::EndDef));
	Keywords.insert(std::make_pair("endfor", Token::EndFor));
	Keywords.insert(std::make_pair("endif", Token::EndIf));
	Keywords.insert(std::make_pair("endwhile", Token::EndWhile));
	Keywords.insert(std::make_pair("for", Token::For));
	Keywords.insert(std::make_pair("if", Token::If));
	Keywords.insert(std::make_pair("in", Token::In));
	Keywords.insert(std::make_pair("not", Token::Not));
	Keywords.insert(std::make_pair("or", Token::Or));
	Keywords.insert(std::make_pair("pass", Token::Pass));
	Keywords.insert(std::make_pair("print", Token::Print));
	Keywords.insert(std::make_pair("return", Token::Return));
	Keywords.insert(std::make_pair("while", Token::While));
}
