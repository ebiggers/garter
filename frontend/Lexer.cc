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

Token Lexer::lexIdentifierOrKeyword()
{
	Token tok;
	const char *start;
	size_t len;
	char *name;

	start = NextCharPtr;
	tok.Type = Token::Identifier;

	do {
		NextCharPtr++;
	} while (CharTab[(uint8_t)*NextCharPtr] & (LOWER_CASE | UPPER_CASE |
						   UNDERSCORE | NUMBER));
	len = NextCharPtr - start;

	name = new char[len + 1];
	memcpy(name, start, len);
	name[len] = '\0';

	auto it = Keywords.find(name);
	if (it == Keywords.end()) {
		tok.Value.Name = name;
	} else {
		tok = it->second;
		delete name;
	}

	return tok;
}

Token Lexer::lexNumber()
{
	Token tok(Token::Number);
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

	tok.Value.Number = n;

	return tok;

too_large:
	fprintf(stderr, "%s: Integer constant on line %lu is "
		"too large!\n", Tag, CurrentLineNumber);
	tok.Type = Token::Error;
	return tok;
}

Token Lexer::getNextToken()
{
	Token tok;

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
		tok = lexIdentifierOrKeyword();
		break;

	case '0' ... '9':
		tok = lexNumber();
		break;

	case '(':
		tok.Type = Token::LeftParenthesis;
		NextCharPtr++;
		break;

	case ')':
		tok.Type = Token::RightParenthesis;
		NextCharPtr++;
		break;

	case ':':
		tok.Type = Token::Colon;
		NextCharPtr++;
		break;

	case ';':
		tok.Type = Token::Semicolon;
		NextCharPtr++;
		break;

	case ',':
		tok.Type = Token::Comma;
		NextCharPtr++;
		break;

	case '[':
		tok.Type = Token::LeftSquareBracket;
		NextCharPtr++;
		break;

	case ']':
		tok.Type = Token::RightSquareBracket;
		NextCharPtr++;
		break;

	case '=':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			// Double equals (equality predicate)
			tok.Type = Token::DoubleEquals;
			NextCharPtr++;
		} else {
			// Equals (assignment)
			tok.Type = Token::Equals;
		}
		break;

	case '+':
		tok.Type = Token::Plus;
		NextCharPtr++;
		break;

	case '-':
		tok.Type = Token::Minus;
		NextCharPtr++;
		break;

	case '*':
		NextCharPtr++;
		if (*NextCharPtr == '*') {
			tok.Type = Token::DoubleAsterisk;
			NextCharPtr++;
		} else {
			tok.Type = Token::Asterisk;
		}
		break;

	case '/':
		tok.Type = Token::ForwardSlash;
		NextCharPtr++;
		break;

	case '%':
		tok.Type = Token::Percent;
		NextCharPtr++;
		break;

	case '<':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			tok.Type = Token::LessThanOrEqualTo;
			NextCharPtr++;
		} else {
			tok.Type = Token::LessThan;
		}
		break;

	case '>':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			tok.Type = Token::GreaterThanOrEqualTo;
			NextCharPtr++;
		} else {
			tok.Type = Token::GreaterThan;
		}
		break;

	case '!':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			// "Not equal to" symbol
			tok.Type = Token::NotEqualTo;
			NextCharPtr++;
		} else {
			// '!' followed by something else--- not valid
			tok.Type = Token::Error;
			fprintf(stderr, "%s: Unexpected character '%c' "
				"after '!' on line %lu\n",
				Tag, *NextCharPtr, CurrentLineNumber);
		}
		break;

	case '\0':
		// '\0' should mark end of buffer
		if (NextCharPtr == Buffer->getBufferEnd()) {
			tok.Type = Token::EndOfFile;
		} else {
			tok.Type = Token::Error;
			fprintf(stderr, "%s: Unexpected embedded null "
				"character on line %lu\n",
				Tag, CurrentLineNumber);
		}
		break;

	default:
		tok.Type = Token::Error;
		fprintf(stderr, "%s: Unexpected character '%c' on line %lu\n",
			Tag, *NextCharPtr, CurrentLineNumber);
		break;
	}
	return tok;
}
