#include "GarterLexer.h"
#include <stdio.h>
#include <string.h>

const char * const GarterLexer::Tag = "GarterLexer";

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

GarterToken GarterLexer::lexIdentifier()
{
	GarterToken tok;
	const char *start;
	size_t len;
	char *name;

	start = NextCharPtr;
	tok.Type = GarterToken::Identifier;

	do {
		NextCharPtr++;
	} while (CharTab[(uint8_t)*NextCharPtr] & (LOWER_CASE | UPPER_CASE |
						   UNDERSCORE | NUMBER));
	len = NextCharPtr - start;

	name = new char[len + 1];
	memcpy(name, start, len);
	name[len] = '\0';
	tok.Value.Name = name;
	return tok;
}

GarterToken GarterLexer::lexNumber()
{
	GarterToken tok(GarterToken::Number);
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
	tok.Type = GarterToken::Error;
	return tok;
}

GarterToken GarterLexer::getNextToken()
{
	GarterToken tok;

next_char:
	switch (*NextCharPtr) {
	case '\n':
		CurrentLineNumber++;
		/* Fall through  */
	case ' ':
	case '\t':
	case '\v':
		/* Skip whitespace character  */
		NextCharPtr++;
		goto next_char;

	case '#':
		/* Skip comment  */
		do {
			NextCharPtr++;
		} while (*NextCharPtr != '\n' && *NextCharPtr != '\0');
		goto next_char;

	case 'a' ... 'z':
	case 'A' ... 'Z':
	case '_':
		tok = lexIdentifier();
		break;

	case '0' ... '9':
		tok = lexNumber();
		break;

	case '(':
		tok.Type = GarterToken::LeftParenthesis;
		NextCharPtr++;
		break;

	case ')':
		tok.Type = GarterToken::RightParenthesis;
		NextCharPtr++;
		break;

	case ':':
		tok.Type = GarterToken::Colon;
		NextCharPtr++;
		break;

	case ';':
		tok.Type = GarterToken::Semicolon;
		NextCharPtr++;
		break;

	case ',':
		tok.Type = GarterToken::Comma;
		NextCharPtr++;
		break;

	case '[':
		tok.Type = GarterToken::LeftSquareBracket;
		NextCharPtr++;
		break;

	case ']':
		tok.Type = GarterToken::RightSquareBracket;
		NextCharPtr++;
		break;

	case '=':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			/* Double equals (equality predicate)  */
			tok.Type = GarterToken::DoubleEquals;
			NextCharPtr++;
		} else {
			/* Equals (assignment)  */
			tok.Type = GarterToken::Equals;
		}
		break;

	case '+':
		tok.Type = GarterToken::Plus;
		NextCharPtr++;
		break;

	case '-':
		tok.Type = GarterToken::Minus;
		NextCharPtr++;
		break;

	case '*':
		NextCharPtr++;
		if (*NextCharPtr == '*') {
			tok.Type = GarterToken::DoubleAsterisk;
			NextCharPtr++;
		} else {
			tok.Type = GarterToken::Asterisk;
		}
		break;

	case '/':
		tok.Type = GarterToken::ForwardSlash;
		NextCharPtr++;
		break;

	case '%':
		tok.Type = GarterToken::Percent;
		NextCharPtr++;
		break;

	case '<':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			tok.Type = GarterToken::LessThanOrEqualTo;
			NextCharPtr++;
		} else {
			tok.Type = GarterToken::LessThan;
		}
		break;

	case '>':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			tok.Type = GarterToken::GreaterThanOrEqualTo;
			NextCharPtr++;
		} else {
			tok.Type = GarterToken::GreaterThan;
		}
		break;

	case '!':
		NextCharPtr++;
		if (*NextCharPtr == '=') {
			/* "Not equal to" symbol  */
			tok.Type = GarterToken::NotEqualTo;
			NextCharPtr++;
		} else {
			/* '!' followed by something else--- not valid  */
			tok.Type = GarterToken::Error;
			fprintf(stderr, "%s: Unexpected character '%c' "
				"after '!' on line %lu\n",
				Tag, *NextCharPtr, CurrentLineNumber);
		}
		break;

	case '\0':
		/* '\0' should mark end of buffer  */
		if (NextCharPtr == Buffer->getBufferEnd()) {
			tok.Type = GarterToken::EndOfFile;
		} else {
			tok.Type = GarterToken::Error;
			fprintf(stderr, "%s: Unexpected embedded null "
				"character on line %lu\n",
				Tag, CurrentLineNumber);
		}
		break;

	default:
		tok.Type = GarterToken::Error;
		fprintf(stderr, "%s: Unexpected character '%c' on line %lu\n",
			Tag, *NextCharPtr, CurrentLineNumber);
		break;
	}
	return tok;
}
