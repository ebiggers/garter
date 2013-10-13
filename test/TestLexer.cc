#include <frontend/Lexer.h>
#include <stdio.h>
#include <stdlib.h>

using namespace garter;

static const struct LexerTestCase {
	const char *Input;
	Token ExpectedOutput[9];
} testcases[] = {
	{
		.Input = "",
		.ExpectedOutput =
		{
			Token(Token::EndOfFile),
		},

	},
	{
		.Input = "(",
		.ExpectedOutput =
		{
			Token(Token::LeftParenthesis),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "():;,[]",
		.ExpectedOutput =
		{
			Token(Token::LeftParenthesis),
			Token(Token::RightParenthesis),
			Token(Token::Colon),
			Token(Token::Semicolon),
			Token(Token::Comma),
			Token(Token::LeftSquareBracket),
			Token(Token::RightSquareBracket),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "0",
		.ExpectedOutput =
		{
			Token(Token::Number, 0),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "0000000000003",
		.ExpectedOutput =
		{
			Token(Token::Number, 3),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "138",
		.ExpectedOutput =
		{
			Token(Token::Number, 138),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "2147483647",
		.ExpectedOutput =
		{
			Token(Token::Number, 2147483647),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "2147483648",
		.ExpectedOutput =
		{
			Token(Token::Error),
		},
	},
	{
		.Input = "a",
		.ExpectedOutput =
		{
			Token(Token::Identifier, "a"),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "_azAZ09",
		.ExpectedOutput =
		{
			Token(Token::Identifier, "_azAZ09"),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "if if0 for while",
		.ExpectedOutput =
		{
			Token(Token::If),
			Token(Token::Identifier, "if0"),
			Token(Token::For),
			Token(Token::While),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "-10**2 + 3/b",
		.ExpectedOutput =
		{
			Token(Token::Minus),
			Token(Token::Number, 10),
			Token(Token::DoubleAsterisk),
			Token(Token::Number, 2),
			Token(Token::Plus),
			Token(Token::Number, 3),
			Token(Token::ForwardSlash),
			Token(Token::Identifier, "b"),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "< > <= >= == !=",
		.ExpectedOutput =
		{
			Token(Token::LessThan),
			Token(Token::GreaterThan),
			Token(Token::LessThanOrEqualTo),
			Token(Token::GreaterThanOrEqualTo),
			Token(Token::DoubleEquals),
			Token(Token::NotEqualTo),
			Token(Token::EndOfFile),
		},
	},
	{
		.Input = "!",
		.ExpectedOutput =
		{
			Token(Token::Error),
		},
	},
	{
		.Input = "\xff",
		.ExpectedOutput =
		{
			Token(Token::Error),
		},
	},
};

#define ARRAY_LEN(A) (sizeof(A) / sizeof((A)[0]))

static void do_test(const LexerTestCase &testcase)
{
	llvm::OwningPtr<llvm::MemoryBuffer> buffer(
			llvm::MemoryBuffer::getMemBuffer(testcase.Input));

	Lexer lexer(buffer.get());

	printf("Test string %s\n", testcase.Input);

	for (size_t j = 0; j < ARRAY_LEN(testcase.ExpectedOutput); j++) {

		const Token &tok = testcase.ExpectedOutput[j];
		Token actual_tok = lexer.getNextToken();

		if (tok.Type != actual_tok.Type) {
			fprintf(stderr, "Input \"%s\": token type mismatch "
				"@ pos %zu\n", testcase.Input, j);
			exit(1);
		}

		if (tok.Type == Token::Number &&
		    tok.Value.Number != actual_tok.Value.Number)
		{
			fprintf(stderr, "Input \"%s\": numeric value mismatch "
				"@ pos %zu (expected %d, got %d)\n",
				testcase.Input, j,
				tok.Value.Number, actual_tok.Value.Number);
			exit(1);
		}

		if (tok.Type == Token::Identifier &&
		    (tok.Value.Name == NULL || actual_tok.Value.Name == NULL ||
		     strcmp(tok.Value.Name, actual_tok.Value.Name)))
		{
			fprintf(stderr, "Input \"%s\": identifier name "
				"mismatch @ pos %zu (expected %s, got %s)\n",
				testcase.Input, j,
				tok.Value.Name, actual_tok.Value.Name);
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
