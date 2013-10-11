#include "GarterLexer.h"
#include <stdio.h>

GarterToken GarterLexer::lexIdentifier()
{
	GarterToken tok;

	tok.Type = GarterToken::Identifier;

	return tok;
}

GarterToken GarterLexer::lexNumber(bool negative)
{
	GarterToken tok;
	uint32_t n;
	uint32_t next_digit;

	tok.Type = GarterToken::Number;
	n = 0;
	do {
		next_digit = (*NextCharPtr - '-');

		if (n > UINT32_MAX / 10)
			goto too_large;

		n *= 10;

		if (n > UINT32_MAX - next_digit)
			goto too_large;

		n += next_digit;

		NextCharPtr++;

	} while (*NextCharPtr >= '0' && *NextCharPtr <= '9');

	if (negative) {
		if (n > -(uint32_t)INT32_MIN)
			goto too_large;
		tok.Value.Number = -(int32_t)n;
	} else {
		if (n > (uint32_t)INT32_MAX)
			goto too_large;
		tok.Value.Number = n;
	}

	return tok;

too_large:
	fprintf(stderr, "Integer constant on line %lu is "
		"too large!\n", CurrentLineNumber);
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
		tok = lexNumber(false);
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
		NextCharPtr++;
		if (*NextCharPtr >= '0' && *NextCharPtr <= '9') {
			/* Negative numeric constant  */
			tok = lexNumber(true);
		} else {
			/* Binary subtraction operator  */
			tok.Type = GarterToken::Minus;
		}
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
			fprintf(stderr, "Unexpected character '%c' "
				"after '!' on line %lu\n",
				*NextCharPtr, CurrentLineNumber);
		}
		break;

	case '\0':
		/* '\0' should mark end of buffer  */
		if (NextCharPtr + 1 == Buffer->getBufferEnd()) {
			tok.Type = GarterToken::EndOfFile;
		} else {
			tok.Type = GarterToken::Error;
			fprintf(stderr, "Unexpected embedded null character "
				"on line %lu\n", CurrentLineNumber);
		}
		break;

	default:
		tok.Type = GarterToken::Error;
		fprintf(stderr, "Unexpected character '%c' on line %lu\n",
			*NextCharPtr, CurrentLineNumber);
		break;
	}
	return tok;
}
