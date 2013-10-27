#include "Lexer.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sstream>

using namespace garter;


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
	case '\r':
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

void Lexer::init()
{
	Keywords["and"]      = Token::And;
	Keywords["break"]    = Token::Break;
	Keywords["continue"] = Token::Continue;
	Keywords["def"]      = Token::Def;
	Keywords["else"]     = Token::Else;
	Keywords["elif"]     = Token::Elif;
	Keywords["enddef"]   = Token::EndDef;
	Keywords["endfor"]   = Token::EndFor;
	Keywords["endif"]    = Token::EndIf;
	Keywords["endwhile"] = Token::EndWhile;
	Keywords["extern"]   = Token::Extern;
	Keywords["for"]      = Token::For;
	Keywords["if"]       = Token::If;
	Keywords["in"]       = Token::In;
	Keywords["not"]      = Token::Not;
	Keywords["or"]       = Token::Or;
	Keywords["pass"]     = Token::Pass;
	Keywords["print"]    = Token::Print;
	Keywords["return"]   = Token::Return;
	Keywords["while"]    = Token::While;

	CurrentLineNumber = 1;
	InputStream->unsetf(std::ios_base::skipws);
	nextChar();

	memset(CharTab, 0, sizeof(CharTab));

	for (uint8_t c = 'a'; c <= 'z'; c++)
		CharTab[c] |= LOWER_CASE;
	for (uint8_t c = 'A'; c <= 'Z'; c++)
		CharTab[c] |= UPPER_CASE;
	CharTab[(uint8_t)'_'] |= UNDERSCORE;
	for (uint8_t c = '0'; c <= '9'; c++)
		CharTab[c] |= NUMBER;
}

Lexer::Lexer(const char *str)
{
	StringStream = new std::istringstream(str, std::ios_base::in);
	InputStream = StringStream;
	init();
}

Lexer::Lexer(std::istream & is)
{
	StringStream = nullptr;
	InputStream = &is;
	init();
}

Lexer::~Lexer()
{
	delete StringStream;
}
