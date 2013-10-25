#include <frontend/Lexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace garter;

static const struct LexerTestCase {
	const char *Input;
	struct ExpectedToken {
		Token::TokenType Type;
		const char *Name;
		int32_t Number;
	} ExpectedOutput[9];
} testcases[] = {
	{
		.Input = "",
		.ExpectedOutput =
		{
			{.Type = Token::EndOfFile},
		},

	},
	{
		.Input = "(",
		.ExpectedOutput =
		{
			{.Type = Token::LeftParenthesis},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "():;,[]",
		.ExpectedOutput =
		{
			{.Type = Token::LeftParenthesis},
			{.Type = Token::RightParenthesis},
			{.Type = Token::Colon},
			{.Type = Token::Semicolon},
			{.Type = Token::Comma},
			{.Type = Token::LeftSquareBracket},
			{.Type = Token::RightSquareBracket},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "0",
		.ExpectedOutput =
		{
			{.Type = Token::Number, .Number = 0},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "0000000000003",
		.ExpectedOutput =
		{
			{.Type = Token::Number, .Number = 3},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "138",
		.ExpectedOutput =
		{
			{.Type = Token::Number, .Number = 138},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "2147483647",
		.ExpectedOutput =
		{
			{.Type = Token::Number, .Number = 2147483647},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "2147483648",
		.ExpectedOutput =
		{
			{.Type = Token::Error},
		},
	},
	{
		.Input = "a",
		.ExpectedOutput =
		{
			{.Type = Token::Identifier, .Name = "a"},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "_azAZ09",
		.ExpectedOutput =
		{
			{.Type = Token::Identifier, .Name = "_azAZ09"},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "if if0 for while",
		.ExpectedOutput =
		{
			{.Type = Token::If},
			{.Type = Token::Identifier, .Name = "if0"},
			{.Type = Token::For},
			{.Type = Token::While},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "-10**2 + 3/b",
		.ExpectedOutput =
		{
			{.Type = Token::Minus},
			{.Type = Token::Number, .Number = 10},
			{.Type = Token::DoubleAsterisk},
			{.Type = Token::Number, .Number = 2},
			{.Type = Token::Plus},
			{.Type = Token::Number, .Number = 3},
			{.Type = Token::ForwardSlash},
			{.Type = Token::Identifier, .Name = "b"},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "< > <= >= == !=",
		.ExpectedOutput =
		{
			{.Type = Token::LessThan},
			{.Type = Token::GreaterThan},
			{.Type = Token::LessThanOrEqualTo},
			{.Type = Token::GreaterThanOrEqualTo},
			{.Type = Token::DoubleEquals},
			{.Type = Token::NotEqualTo},
			{.Type = Token::EndOfFile},
		},
	},
	{
		.Input = "!",
		.ExpectedOutput =
		{
			{.Type = Token::Error},
		},
	},
	{
		.Input = "\xff",
		.ExpectedOutput =
		{
			{.Type = Token::Error},
		},
	},
};

#define ARRAY_LEN(A) (sizeof(A) / sizeof((A)[0]))

static void do_test(const LexerTestCase &testcase)
{
	Lexer lexer(testcase.Input);

	printf("Test string %s\n", testcase.Input);

	for (size_t j = 0; j < ARRAY_LEN(testcase.ExpectedOutput); j++) {

		const LexerTestCase::ExpectedToken &tok =
				testcase.ExpectedOutput[j];
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
