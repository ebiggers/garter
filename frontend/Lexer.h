#include <inttypes.h>
#include <limits.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>
#include <stdio.h>
#include <map>
#include <set>
#include <memory>

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


	explicit Token(enum TokenType type)
		: Type(type) { }

	explicit Token(enum TokenType type, char *name)
		: Type(type), Name(name) { }

	explicit Token(enum TokenType type, int32_t number) 
		: Type(type), _Number(number) { }
};

class Lexer {
private:
	std::shared_ptr<llvm::MemoryBuffer> Buffer;
	unsigned long CurrentLineNumber;
	const char *NextCharPtr;
	std::map<std::string, Token::TokenType> Keywords;

	static const char * const Tag;

	std::unique_ptr<Token> lexNumber();
	std::unique_ptr<Token> lexIdentifierOrKeyword();

public:
	explicit Lexer(std::shared_ptr<llvm::MemoryBuffer> buffer)
		: Buffer(buffer),
		  CurrentLineNumber(1)
	{
		NextCharPtr = buffer->getBufferStart();
		Keywords.insert(std::make_pair("and", Token::And));
		Keywords.insert(std::make_pair("def", Token::Def));
		Keywords.insert(std::make_pair("else", Token::Else));
		Keywords.insert(std::make_pair("elif", Token::Elif));
		Keywords.insert(std::make_pair("enddef", Token::EndDef));
		Keywords.insert(std::make_pair("endfor", Token::EndFor));
		Keywords.insert(std::make_pair("endif", Token::EndIf));
		Keywords.insert(std::make_pair("endwhile", Token::EndWhile));
		Keywords.insert(std::make_pair("for", Token::For));
		Keywords.insert(std::make_pair("if", Token::If));
		Keywords.insert(std::make_pair("in", Token::In));
		Keywords.insert(std::make_pair("not", Token::Not));
		Keywords.insert(std::make_pair("or", Token::Or));
		Keywords.insert(std::make_pair("pass", Token::Pass));
		Keywords.insert(std::make_pair("print", Token::Print));
		Keywords.insert(std::make_pair("return", Token::Return));
		Keywords.insert(std::make_pair("while", Token::While));
	}

	~Lexer() {
	}

	std::unique_ptr<Token> getNextToken();
};

} // End garter namespace
