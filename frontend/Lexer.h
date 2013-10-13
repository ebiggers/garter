#include <inttypes.h>
#include <limits.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>
#include <stdio.h>
#include <map>

namespace garter {

class Token {
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

	explicit Token(enum TokenType type) {
		Type = type;
		Value.Name = NULL;
	}

	explicit Token(enum TokenType type, const char *name) {
		Type = type;
		Value.Name = name;
	}

	explicit Token(enum TokenType type, int32_t number) {
		Type = type;
		Value.Number = number;
	}

	explicit Token() {
		Type = Token::Error;
		Value.Name = NULL;
	}

	~Token() {
	}
};

class Lexer {
private:
	llvm::MemoryBuffer *Buffer;
	unsigned long CurrentLineNumber;
	const char *NextCharPtr;
	std::map<std::string, Token> Keywords;

	static const char * const Tag;

	Token lexNumber();
	Token lexIdentifierOrKeyword();

public:
	explicit Lexer(llvm::MemoryBuffer *buffer)
		: Buffer(buffer),
		  CurrentLineNumber(1)
	{
		NextCharPtr = buffer->getBufferStart();
		Keywords["and"] = Token(Token::And);
		Keywords["def"] = Token(Token::Def);
		Keywords["else"] = Token(Token::Else);
		Keywords["elif"] = Token(Token::Elif);
		Keywords["enddef"] = Token(Token::EndDef);
		Keywords["endfor"] = Token(Token::EndFor);
		Keywords["endif"] = Token(Token::EndIf);
		Keywords["endwhile"] = Token(Token::EndWhile);
		Keywords["for"] = Token(Token::For);
		Keywords["if"] = Token(Token::If);
		Keywords["in"] = Token(Token::In);
		Keywords["not"] = Token(Token::Not);
		Keywords["or"] = Token(Token::Or);
		Keywords["pass"] = Token(Token::Pass);
		Keywords["print"] = Token(Token::Print);
		Keywords["return"] = Token(Token::Return);
		Keywords["while"] = Token(Token::While);
	}

	~Lexer() {
	}

	Token getNextToken();
};

} // End garter namespace
