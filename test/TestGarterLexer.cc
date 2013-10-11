#include <frontend/GarterLexer.h>
#include <stdio.h>
#include <stdlib.h>

static const struct LexerTestCase {
	const char *Input;
	GarterToken ExpectedOutput[8];
} testcases[] = {
	{
		.Input = "",
		.ExpectedOutput =
		{
			GarterToken(GarterToken::EndOfFile),
		},
	},
};

#define ARRAY_LEN(A) (sizeof(A) / sizeof((A)[0]))

static void do_test(const LexerTestCase &testcase)
{
	llvm::MemoryBuffer *buffer;
	
	buffer = llvm::MemoryBuffer::getMemBuffer(testcase.Input);

	GarterLexer lexer(buffer);

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
				"@ pos %zu\n", testcase.Input, j);
			exit(1);
		}

		if (tok.Type == GarterToken::EndOfFile ||
		    tok.Type == GarterToken::Error)
			break;
	}

	delete buffer;
}

int main()
{
	for (size_t i = 0; i < ARRAY_LEN(testcases); i++)
		do_test(testcases[i]);

	return 0;
}
