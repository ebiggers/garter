#ifndef _GARTER_LEXER_H_
#define _GARTER_LEXER_H_

#include <inttypes.h>
#include <map>
#include <memory>
#include <assert.h>

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
		NotIn,
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
	};

	TokenType getType() const {
		return Type;
	}

	const char * getName() const {
		assert(Type == Identifier);
		return Name;
	};

	int32_t getNumber() const {
		assert(Type == Number);
		return _Number;
	};

	~Token() {
		if (Type == TokenType::Identifier) {
			delete[] Name;
		}
	}

private:

	friend class Lexer;

	TokenType Type;
	char *Name;
	int32_t _Number;


	Token(enum TokenType type)
		: Type(type) { }

	Token(enum TokenType type, char *name)
		: Type(type), Name(name) { }

	Token(enum TokenType type, int32_t number) 
		: Type(type), _Number(number) { }
};

class Lexer {
private:
	const char *NextCharPtr;
	unsigned long CurrentLineNumber;
	std::map<std::string, Token::TokenType> Keywords;

	static const char * const Tag;

	std::unique_ptr<Token> lexNumber();
	std::unique_ptr<Token> lexIdentifierOrKeyword();

public:
	Lexer(const char *string);
	std::unique_ptr<Token> getNextToken();
	void reportError(const char *msg, ...);
};

} // End garter namespace

#endif /* _GARTER_LEXER_H_ */
