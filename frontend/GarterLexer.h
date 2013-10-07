#include <inttypes.h>
#include <limits.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>

using namespace llvm;

class GarterToken {
public:
	enum {
		tok_identifier,
		tok_number,
		tok_left_paren,
		tok_right_paren,
		tok_colon,
		tok_semicolon,
		tok_comma,
		tok_left_square_bracket,
		tok_right_square_bracket,
		tok_equals,
		tok_plus,
		tok_minus,
		tok_asterisk,
		tok_forward_slash,
		tok_percent,
		tok_double_asterisk,
		tok_less_than,
		tok_greater_than,
		tok_less_than_or_equal_to,
		tok_greater_than_or_equal_to,
		tok_double_equals,
		tok_not_equal_to,
		tok_eof,
	} type;
	union {
		int32_t number;
		const char *errorMsg;
	} val;
};

class GarterLexer {
private:
	OwningPtr<MemoryBuffer> buffer;
	const char *nextChar;
	unsigned linesRead;

	GarterToken lexNumber(bool);
	GarterToken lexIdentifier();

public:
	GarterLexer() { }
	~GarterLexer() { }
	
	error_code init(StringRef filename);

	GarterToken getNextToken();
};
