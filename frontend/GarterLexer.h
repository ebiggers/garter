#include <inttypes.h>
#include <limits.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>
#include <stdio.h>
#include <map>

class GarterToken {
public:
	enum TokenType {
		EndOfFile = -1,
		Error = 0,
		And,
		Asterisk,
		Colon,
		Comma,
		Def,
		DoubleAsterisk,
		DoubleEquals,
		Elif,
		Else,
		EndDef,
		EndFor,
		EndIf,
		EndWhile,
		Equals,
		For,
		ForwardSlash,
		GreaterThan,
		GreaterThanOrEqualTo,
		Identifier,
		If,
		In,
		LeftParenthesis,
		LeftSquareBracket,
		LessThan,
		LessThanOrEqualTo,
		Minus,
		Not,
		NotEqualTo,
		Number,
		Or,
		Pass,
		Percent,
		Plus,
		Print,
		Return,
		RightParenthesis,
		RightSquareBracket,
		Semicolon,
		While,
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
	unsigned long CurrentLineNumber;
	const char *NextCharPtr;
	std::map<std::string, GarterToken> Keywords;

	static const char * const Tag;

	GarterToken lexNumber();
	GarterToken lexIdentifierOrKeyword();

public:
	GarterLexer(llvm::MemoryBuffer *buffer)
		: Buffer(buffer),
		  CurrentLineNumber(1)
	{
		NextCharPtr = buffer->getBufferStart();
		Keywords["and"] = GarterToken(GarterToken::And);
		Keywords["def"] = GarterToken(GarterToken::Def);
		Keywords["else"] = GarterToken(GarterToken::Else);
		Keywords["elif"] = GarterToken(GarterToken::Elif);
		Keywords["enddef"] = GarterToken(GarterToken::EndDef);
		Keywords["endfor"] = GarterToken(GarterToken::EndFor);
		Keywords["endif"] = GarterToken(GarterToken::EndIf);
		Keywords["endwhile"] = GarterToken(GarterToken::EndWhile);
		Keywords["for"] = GarterToken(GarterToken::For);
		Keywords["if"] = GarterToken(GarterToken::If);
		Keywords["in"] = GarterToken(GarterToken::In);
		Keywords["not"] = GarterToken(GarterToken::Not);
		Keywords["or"] = GarterToken(GarterToken::Or);
		Keywords["pass"] = GarterToken(GarterToken::Pass);
		Keywords["print"] = GarterToken(GarterToken::Print);
		Keywords["return"] = GarterToken(GarterToken::Return);
		Keywords["while"] = GarterToken(GarterToken::While);
	}

	~GarterLexer() {
	}

	GarterToken getNextToken();
};
