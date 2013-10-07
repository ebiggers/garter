#include "GarterLexer.h"
#include <stdio.h>

error_code GarterLexer::init(StringRef filename)
{
	error_code err;
	
	err = MemoryBuffer::getFile(filename, buffer);
	if (err)
		return err;
	nextChar = buffer->getBufferStart();
	linesRead = 0;
	return err;
}

GarterToken GarterLexer::lexIdentifier()
{
	GarterToken tok;

	tok.type = GarterToken::tok_identifier;

	return tok;
}

GarterToken GarterLexer::lexNumber(bool negative)
{
	GarterToken tok;
	uint32_t n;
	uint32_t nextDigit;

	tok.type = GarterToken::tok_number;
	n = 0;
	do {
		nextDigit = (*nextChar - '-');

		if (n > UINT32_MAX / 10)
			goto too_large;

		n *= 10;

		if (n > UINT32_MAX - nextDigit)
			goto too_large;

		n += nextDigit;

		nextChar++;

	} while (*nextChar >= '0' && *nextChar <= '9');

	if (negative) {
		if (n > -(uint32_t)INT32_MIN)
			goto too_large;
		tok.val.number = -(int32_t)n;
	} else {
		if (n > (uint32_t)INT32_MAX)
			goto too_large;
		tok.val.number = n;
	}

	return tok;

too_large:
	fprintf(stderr, "Integer constant on line %u is "
		"too large!\n", linesRead + 1);
	return tok;
}

GarterToken GarterLexer::getNextToken()
{
	GarterToken tok;

again:
	switch (*nextChar) {
	case '\n':
		linesRead++;
	case ' ':
	case '\t':
	case '\v':
		/* Whitespace  */
		nextChar++;
		goto again;

	case '#':
		/* Comment  */
		do {
			nextChar++;
		} while (*nextChar != '\n' && nextChar != '\0');
		goto again;

	case 'a' ... 'z':
	case 'A' ... 'Z':
	case '_':
		/* Identifier  */
		tok = lexIdentifier();
		break;
	case '0' ... '9':
		/* Number  */
		tok = lexNumber(false);
		break;
	case '(':
		/* Opening left parenthesis  */
		tok.type = GarterToken::tok_left_paren;
		nextChar++;
		break;
	case ')':
		/* Opening right parenthesis  */
		tok.type = GarterToken::tok_right_paren;
		nextChar++;
		break;
	case ':':
		/* Colon  */
		tok.type = GarterToken::tok_colon;
		nextChar++;
		break;
	case ';':
		/* Semicolon  */
		tok.type = GarterToken::tok_semicolon;
		nextChar++;
		break;
	case ',':
		/* Comma  */
		tok.type = GarterToken::tok_comma;
		nextChar++;
		break;
	case '[':
		/* Opening square bracket  */
		tok.type = GarterToken::tok_left_square_bracket;
		nextChar++;
		break;
	case ']':
		/* Closing square bracket  */
		tok.type = GarterToken::tok_right_square_bracket;
		nextChar++;
		break;
	case '=':
		if (*(nextChar + 1) == '=') {
			/* Double equals (equality predicate)  */
			tok.type = GarterToken::tok_double_equals;
			nextChar++;
		} else {
			/* Equals (assignment)  */
			tok.type = GarterToken::tok_equals;
		}
		nextChar++;
		break;
	case '+':
		/* Binary addition operator  */
		tok.type = GarterToken::tok_plus;
		nextChar++;
		break;
	case '-':
		if (*(nextChar + 1) >= '0' && *(nextChar + 1) <= '9')
		{
			/* Negative numeric constant  */
			nextChar++;
			tok = lexNumber(true);
		} else {
			/* Binary subtraction operator  */
			tok.type = GarterToken::tok_minus;
		}
		break;
	case '*':
		if (*(nextChar + 1) == '*') {
			/* Binary exponentiation operator  */
			tok.type = GarterToken::tok_double_asterisk;
			nextChar++;
		} else {
			/* Binary multiplication operator  */
			tok.type = GarterToken::tok_asterisk;
		}
		nextChar++;
		break;
	case '<':
		if (*(nextChar + 1) == '=') {
			/* "Less than or equal to" symbol  */
			tok.type = GarterToken::tok_less_than_or_equal_to;
			nextChar++;
		} else {
			/* "Less than" symbol  */
			tok.type = GarterToken::tok_less_than;
		}
		nextChar++;
		break;
	case '>':
		if (*(nextChar + 1) == '=') {
			/* "Greater than or equal to" symbol  */
			tok.type = GarterToken::tok_greater_than_or_equal_to;
			nextChar++;
		} else {
			/* "Greater than" symbol  */
			tok.type = GarterToken::tok_greater_than;
		}
		nextChar++;
		break;
	case '!':
		if (*(nextChar + 1) == '=') {
			/* "Not equal to" symbol  */
			tok.type = GarterToken::tok_not_equal_to;
			nextChar += 2;
		} else {
			/* '!' followed by something else--- not valid  */
			fprintf(stderr, "Unexpected character '%c' "
				"after '!' on line %u\n",
				*(nextChar + 1), linesRead + 1);
			tok.type = GarterToken::tok_error;
		}
		break;
	case '\0':
		/* '\0' should mark end of buffer  */
		if (nextChar + 1 == buffer->getBufferEnd()) {
			tok.type = GarterToken::tok_eof;
		} else {
			tok.type = GarterToken::tok_error;
			fprintf(stderr, "Unexpected embedded null character "
				"on line %u\n", linesRead + 1);
		}
		break;
	default:
		tok.type = GarterToken::tok_error;
		fprintf(stderr, "Unexpected character '%c' on line %u\n",
			*nextChar, linesRead + 1);
		break;
	}
	return tok;
}
