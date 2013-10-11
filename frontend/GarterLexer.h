#include <inttypes.h>
#include <limits.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>

class GarterToken {
public:
	enum TokenType {
		Error,
		Identifier,
		Number,
		LeftParenthesis,
		RightParenthesis,
		Colon,
		Semicolon,
		Comma,
		LeftSquareBracket,
		RightSquareBracket,
		Equals,
		Plus,
		Minus,
		Asterisk,
		ForwardSlash,
		Percent,
		DoubleAsterisk,
		LessThan,
		GreaterThan,
		LessThanOrEqualTo,
		GreaterThanOrEqualTo,
		DoubleEquals,
		NotEqualTo,
		EndOfFile,
	} Type;

	union {
		int32_t Number;
	} Value;

	GarterToken(enum TokenType type) {
		Type = type;
		Value.Number = 0;
	}

	GarterToken() {
		Type = GarterToken::Error;
		Value.Number = 0;
	}

	~GarterToken() {
	}
};

class GarterLexer {
private:
	llvm::MemoryBuffer *Buffer;
	const char *NextCharPtr;
	unsigned long CurrentLineNumber;

	GarterToken lexNumber(bool negative);
	GarterToken lexIdentifier();

public:
	GarterLexer(llvm::MemoryBuffer *buffer) : Buffer(buffer) { }
	~GarterLexer() { }

	GarterToken getNextToken();
};
