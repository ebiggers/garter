#include <inttypes.h>
#include <limits.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>
#include <stdio.h>

class GarterToken {
public:
	enum TokenType {
		EndOfFile = -1,
		Error = 0,
		Asterisk,
		Colon,
		Comma,
		DoubleAsterisk,
		DoubleEquals,
		Equals,
		ForwardSlash,
		GreaterThan,
		GreaterThanOrEqualTo,
		Identifier,
		LeftParenthesis,
		LeftSquareBracket,
		LessThan,
		LessThanOrEqualTo,
		Minus,
		NotEqualTo,
		Number,
		Percent,
		Plus,
		RightParenthesis,
		RightSquareBracket,
		Semicolon,
	} Type;

	union {
		int32_t Number;
		const char *Name;
	} Value;

	explicit GarterToken(enum TokenType type) {
		Type = type;
		Value.Name = NULL;
	}

	explicit GarterToken(enum TokenType type, const char *name) {
		Type = type;
		Value.Name = name;
	}

	explicit GarterToken(enum TokenType type, int32_t number) {
		Type = type;
		Value.Number = number;
	}

	explicit GarterToken() {
		Type = GarterToken::Error;
		Value.Name = NULL;
	}

	~GarterToken() {
	}
};

class GarterLexer {
private:
	llvm::MemoryBuffer *Buffer;
	const char *NextCharPtr;
	unsigned long CurrentLineNumber;

	static const char * const Tag;

	GarterToken lexNumber();
	GarterToken lexIdentifier();

public:
	GarterLexer(llvm::MemoryBuffer *buffer) {
		Buffer = buffer;
		NextCharPtr = buffer->getBufferStart();
		CurrentLineNumber = 1;
	}

	~GarterLexer() {
	}

	GarterToken getNextToken();
};
