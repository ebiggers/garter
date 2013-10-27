#include <frontend/Lexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace garter;

struct ExpectedToken {
	Token::TokenType Type;
	union {
		const char *Name;
		int32_t Number;
	};

	ExpectedToken()
	{ Type = Token::EndOfFile; }

	ExpectedToken(Token::TokenType type)
	{ Type = type; }

	ExpectedToken(const char *name)
	{ Type = Token::Identifier; Name = name; }

	ExpectedToken(int32_t number)
	{ Type = Token::Number; Number = number; }
};

static const struct LexerTestCase {
	const char *Input;
	struct ExpectedToken ExpectedOutput[9];
} testcases[] = {
	{
		.Input = "",
		.ExpectedOutput =
		{
		},
	},
	{
		.Input = "(",
		.ExpectedOutput =
		{
			ExpectedToken(Token::LeftParenthesis),
		},
	},
	{
		.Input = "():;,[]",
		.ExpectedOutput =
		{
			ExpectedToken(Token::LeftParenthesis),
			ExpectedToken(Token::RightParenthesis),
			ExpectedToken(Token::Colon),
			ExpectedToken(Token::Semicolon),
			ExpectedToken(Token::Comma),
			ExpectedToken(Token::LeftSquareBracket),
			ExpectedToken(Token::RightSquareBracket),
		},
	},
	{
		.Input = "0",
		.ExpectedOutput =
		{
			ExpectedToken(0),
		},
	},
	{
		.Input = "0000000000003",
		.ExpectedOutput =
		{
			ExpectedToken(3),
		},
	},
	{
		.Input = "138",
		.ExpectedOutput =
		{
			ExpectedToken(138),
		},
	},
	{
		.Input = "2147483647",
		.ExpectedOutput =
		{
			ExpectedToken(2147483647),
		},
	},
	{
		.Input = "2147483648",
		.ExpectedOutput =
		{
			ExpectedToken(Token::Error),
		},
	},
	{
		.Input = "a",
		.ExpectedOutput =
		{
			ExpectedToken("a"),
		},
	},
	{
		.Input = "_azAZ09",
		.ExpectedOutput =
		{
			ExpectedToken("_azAZ09"),
		},
	},
	{
		.Input = "if if0 for while",
		.ExpectedOutput =
		{
			ExpectedToken(Token::If),
			ExpectedToken("if0"),
			ExpectedToken(Token::For),
			ExpectedToken(Token::While),
		},
	},
	{
		.Input = "-10**2 + 3/b",
		.ExpectedOutput =
		{
			ExpectedToken(Token::Minus),
			ExpectedToken(10),
			ExpectedToken(Token::DoubleAsterisk),
			ExpectedToken(2),
			ExpectedToken(Token::Plus),
			ExpectedToken(3),
			ExpectedToken(Token::ForwardSlash),
			ExpectedToken("b"),
		},
	},
	{
		.Input = "< > <= >= == !=",
		.ExpectedOutput =
		{
			ExpectedToken(Token::LessThan),
			ExpectedToken(Token::GreaterThan),
			ExpectedToken(Token::LessThanOrEqualTo),
			ExpectedToken(Token::GreaterThanOrEqualTo),
			ExpectedToken(Token::DoubleEquals),
			ExpectedToken(Token::NotEqualTo),
		},
	},
	{
		.Input = "!",
		.ExpectedOutput =
		{
			ExpectedToken(Token::Error),
		},
	},
	{
		.Input = "\xff",
		.ExpectedOutput =
		{
			ExpectedToken(Token::Error),
		},
	},
};

#define ARRAY_LEN(A) (sizeof(A) / sizeof((A)[0]))

static void do_test(const LexerTestCase &testcase)
{
	Lexer lexer(testcase.Input);

	printf("Test string %s\n", testcase.Input);

	for (size_t j = 0; j < ARRAY_LEN(testcase.ExpectedOutput); j++) {

		const ExpectedToken &tok = testcase.ExpectedOutput[j];
		std::unique_ptr<Token> actual_tok = lexer.getNextToken();

		if (tok.Type != actual_tok->getType()) {
			fprintf(stderr, "Input \"%s\": token type mismatch "
				"@ pos %zu\n", testcase.Input, j);
			exit(1);
		}

		if (tok.Type == Token::Number &&
		    tok.Number != actual_tok->getNumber())
		{
			fprintf(stderr, "Input \"%s\": numeric value mismatch "
				"@ pos %zu (expected %d, got %d)\n",
				testcase.Input, j,
				tok.Number, actual_tok->getNumber());
			exit(1);
		}

		if (tok.Type == Token::Identifier &&
		    (tok.Name == nullptr || actual_tok->getName() == nullptr ||
		     strcmp(tok.Name, actual_tok->getName())))
		{
			fprintf(stderr, "Input \"%s\": identifier name "
				"mismatch @ pos %zu (expected %s, got %s)\n",
				testcase.Input, j,
				tok.Name, actual_tok->getName());
			exit(1);
		}

		if (tok.Type == Token::EndOfFile ||
		    tok.Type == Token::Error)
		{
			printf("Break after %zu tokens scanned\n", j + 1);
			break;
		}
	}
}

int main()
{
	for (size_t i = 0; i < ARRAY_LEN(testcases); i++)
		do_test(testcases[i]);

	printf("=======================================\n");
	printf("  TestLexer:  All tests passed!\n");
	printf("=======================================\n");

	return 0;
}
