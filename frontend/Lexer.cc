#include "Lexer.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sstream>

using namespace garter;

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

void Lexer::reportError(const char *format, ...)
{
	va_list va;

	va_start(va, format);
	fprintf(stderr, "Error near line %lu: ", CurrentLineNumber);
	vfprintf(stderr, format, va);
	putc('\n', stderr);
	va_end(va);
}

void Lexer::nextChar()
{
	CurrentChar = 0;
	(*InputStream) >> CurrentChar;
}

std::unique_ptr<Token> Lexer::lexIdentifierOrKeyword()
{
	std::string name;
	do {
		name.push_back(CurrentChar);
		nextChar();
	} while (CharTab[(uint8_t)CurrentChar] & (LOWER_CASE | UPPER_CASE |
						  UNDERSCORE | NUMBER));
	auto it_kw = Keywords.find(name);

	if (it_kw != Keywords.end()) {
		return std::unique_ptr<Token>(new Token(it_kw->second));
	} else {
		char *nameptr = new char[name.length() + 1];
		memcpy(nameptr, name.c_str(), name.length() + 1);

		return std::unique_ptr<Token>(new Token(Token::Identifier, nameptr));
	}
}

std::unique_ptr<Token> Lexer::lexNumber()
{
	int32_t n = 0;

	do {
		int32_t next_digit = (CurrentChar - '0');

		if (n > INT32_MAX / 10)
			goto too_large;

		n *= 10;

		if (n > INT32_MAX - next_digit)
			goto too_large;

		n += next_digit;

		nextChar();

	} while (CurrentChar >= '0' && CurrentChar <= '9');

	return std::unique_ptr<Token>(new Token(Token::Number, n));

too_large:
	reportError("integer constant too large");
	return std::unique_ptr<Token>(new Token(Token::Error));
}

std::unique_ptr<Token> Lexer::getNextToken()
{
next_char:
	switch (CurrentChar) {
	case '\n':
		CurrentLineNumber++;
		// Fall through
	case ' ':
	case '\t':
	case '\v':
		// Skip whitespace character
		nextChar();
		goto next_char;

	case '#':
		// Skip comment
		do {
			nextChar();
		} while (CurrentChar != '\n' && CurrentChar != '\0');
		goto next_char;

	case 'a' ... 'z':
	case 'A' ... 'Z':
	case '_':
		return lexIdentifierOrKeyword();

	case '0' ... '9':
		return lexNumber();

	case '(':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::LeftParenthesis));

	case ')':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::RightParenthesis));

	case ':':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::Colon));

	case ';':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::Semicolon));

	case ',':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::Comma));

	case '[':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::LeftSquareBracket));

	case ']':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::RightSquareBracket));

	case '=':
		nextChar();
		if (CurrentChar == '=') {
			// Double equals (equality predicate)
			nextChar();
			return std::unique_ptr<Token>(new Token(Token::DoubleEquals));
		} else {
			// Equals (assignment)
			return std::unique_ptr<Token>(new Token(Token::Equals));
		}
		break;

	case '+':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::Plus));

	case '-':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::Minus));

	case '*':
		nextChar();
		if (CurrentChar == '*') {
			nextChar();
			return std::unique_ptr<Token>(new Token(Token::DoubleAsterisk));
		} else {
			return std::unique_ptr<Token>(new Token(Token::Asterisk));
		}
		break;

	case '/':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::ForwardSlash));

	case '%':
		nextChar();
		return std::unique_ptr<Token>(new Token(Token::Percent));

	case '<':
		nextChar();
		if (CurrentChar == '=') {
			nextChar();
			return std::unique_ptr<Token>(new Token(Token::LessThanOrEqualTo));
		} else {
			return std::unique_ptr<Token>(new Token(Token::LessThan));
		}

	case '>':
		nextChar();
		if (CurrentChar == '=') {
			nextChar();
			return std::unique_ptr<Token>(new Token(Token::GreaterThanOrEqualTo));
		} else {
			return std::unique_ptr<Token>(new Token(Token::GreaterThan));
		}

	case '!':
		nextChar();
		if (CurrentChar == '=') {
			// "Not equal to" symbol
			nextChar();
			return std::unique_ptr<Token>(new Token(Token::NotEqualTo));
		} else {
			// '!' followed by something else--- not valid
			reportError("unexpected character '%c' after '!'");
			return std::unique_ptr<Token>(new Token(Token::Error));
		}

	case '\0':
		if (InputStream->eof()) {
			return std::unique_ptr<Token>(new Token(Token::EndOfFile));
		} else {
			reportError("error reading input");
			return std::unique_ptr<Token>(new Token(Token::Error));
		}

	default:
		reportError("unexpected character '%c'", CurrentChar);
		return std::unique_ptr<Token>(new Token(Token::Error));
	}
}

Lexer::Lexer()
	: CurrentLineNumber(1),
	  Keywords(),
	  CurrentChar(0),
	  InputStream(nullptr),
	  StringStream(nullptr)
{
	Keywords.insert(std::make_pair("and", Token::And));
	Keywords.insert(std::make_pair("def", Token::Def));
	Keywords.insert(std::make_pair("else", Token::Else));
	Keywords.insert(std::make_pair("elif", Token::Elif));
	Keywords.insert(std::make_pair("enddef", Token::EndDef));
	Keywords.insert(std::make_pair("endfor", Token::EndFor));
	Keywords.insert(std::make_pair("endif", Token::EndIf));
	Keywords.insert(std::make_pair("endwhile", Token::EndWhile));
	Keywords.insert(std::make_pair("extern", Token::Extern));
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

Lexer::Lexer(const char *str)
	: Lexer()
{
	InputStream = StringStream = new std::istringstream(str, std::ios_base::in);
	InputStream->unsetf(std::ios_base::skipws);
	nextChar();
}

Lexer::Lexer(std::istream & is)
	: Lexer()
{
	InputStream = &is;
	nextChar();
}

Lexer::~Lexer()
{
	delete StringStream;
}
