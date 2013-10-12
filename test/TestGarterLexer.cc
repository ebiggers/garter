#include <frontend/GarterLexer.h>
#include <stdio.h>
#include <stdlib.h>

static const struct LexerTestCase {
	const char *Input;
	GarterToken ExpectedOutput[9];
} testcases[] = {
	{
		.Input = "",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::EndOfFile),
		},

	},
	{
		.Input = "(",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::LeftParenthesis),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "():;,[]",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::LeftParenthesis),
			GarterToken(GarterToken::RightParenthesis),
			GarterToken(GarterToken::Colon),
			GarterToken(GarterToken::Semicolon),
			GarterToken(GarterToken::Comma),
			GarterToken(GarterToken::LeftSquareBracket),
			GarterToken(GarterToken::RightSquareBracket),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "0",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Number, 0),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "0000000000003",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Number, 3),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "138",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Number, 138),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "2147483647",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Number, 2147483647),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "2147483648",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Error),
		},
	},
	{
		.Input = "a",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Identifier, "a"),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "_azAZ09",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Identifier, "_azAZ09"),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "-10**2 + 3/b",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Minus),
			GarterToken(GarterToken::Number, 10),
			GarterToken(GarterToken::DoubleAsterisk),
			GarterToken(GarterToken::Number, 2),
			GarterToken(GarterToken::Plus),
			GarterToken(GarterToken::Number, 3),
			GarterToken(GarterToken::ForwardSlash),
			GarterToken(GarterToken::Identifier, "b"),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "< > <= >= == !=",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::LessThan),
			GarterToken(GarterToken::GreaterThan),
			GarterToken(GarterToken::LessThanOrEqualTo),
			GarterToken(GarterToken::GreaterThanOrEqualTo),
			GarterToken(GarterToken::DoubleEquals),
			GarterToken(GarterToken::NotEqualTo),
			GarterToken(GarterToken::EndOfFile),
		},
	},
	{
		.Input = "!",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Error),
		},
	},
	{
		.Input = "\xff",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::Error),
		},
	},
};

#define ARRAY_LEN(A) (sizeof(A) / sizeof((A)[0]))

static void do_test(const LexerTestCase &testcase)
{
	llvm::OwningPtr<llvm::MemoryBuffer> buffer(
			llvm::MemoryBuffer::getMemBuffer(testcase.Input));

	GarterLexer lexer(buffer.get());

	printf("Test string %s\n", testcase.Input);

	for (size_t j = 0; j < ARRAY_LEN(testcase.ExpectedOutput); j++) {

		const GarterToken &tok = testcase.ExpectedOutput[j];
		GarterToken actual_tok = lexer.getNextToken();

		if (tok.Type != actual_tok.Type) {
			fprintf(stderr, "Input \"%s\": token type mismatch "
				"@ pos %zu\n", testcase.Input, j);
			exit(1);
		}

		if (tok.Type == GarterToken::Number &&
		    tok.Value.Number != actual_tok.Value.Number)
		{
			fprintf(stderr, "Input \"%s\": numeric value mismatch "
				"@ pos %zu (expected %d, got %d)\n",
				testcase.Input, j,
				tok.Value.Number, actual_tok.Value.Number);
			exit(1);
		}

		if (tok.Type == GarterToken::Identifier &&
		    (tok.Value.Name == NULL || actual_tok.Value.Name == NULL ||
		     strcmp(tok.Value.Name, actual_tok.Value.Name)))
		{
			fprintf(stderr, "Input \"%s\": identifier name "
				"mismatch @ pos %zu (expected %s, got %s)\n",
				testcase.Input, j,
				tok.Value.Name, actual_tok.Value.Name);
			exit(1);
		}

		if (tok.Type == GarterToken::EndOfFile ||
		    tok.Type == GarterToken::Error)
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
	printf("  TestGarterLexer:  All tests passed!\n");
	printf("=======================================\n");

	return 0;
}
