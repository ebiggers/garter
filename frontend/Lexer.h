#ifndef _GARTER_LEXER_H_
#define _GARTER_LEXER_H_

#include <inttypes.h>
#include <map>
#include <memory>
#include <assert.h>
#include <istream>

namespace garter {

// Representation of a syntactical element in the garter language.
class Token {
public:
	// All possible token types
	enum TokenType {
		// Special end-of-file token
		EndOfFile = -1,

		// Special error token
		Error = 0,

		And,
		Asterisk,
		Break,
		Colon,
		Comma,
		Continue,
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
		Extern,
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

	// Retrieve the type of a Token
	TokenType getType() const {
		return Type;
	}

	// Retrieve the name of a Token (only valid for Identifier Tokens)
	const char * getName() const {
		assert(Type == Identifier);
		return Name;
	};

	// Retrieve the numeric value of a Token (only valid for Number Tokens)
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

// Lexer for the garter language.  This class is responsible for scanning the
// raw sequence of characters making up a garter program and returning Token
// objects that represent syntactical elements.
class Lexer {
private:
	unsigned long CurrentLineNumber;
	std::map<std::string, Token::TokenType> Keywords;
	unsigned char CurrentChar;
	std::istream *InputStream;
	std::istringstream *StringStream;

	std::unique_ptr<Token> lexNumber();
	std::unique_ptr<Token> lexIdentifierOrKeyword();
	void nextChar();

	void init();

public:
	// Create a Lexer that reads characters from the specified
	// null-terminated string.
	Lexer(const char *str);

	// Create a Lexer that reads characters from the specified input stream.
	Lexer(std::istream & is);

	~Lexer();

	// Retrieves the next token in the input.
	//
	// Special values:
	//
	//   - A token of type Token::EndOfFile is returned at the end of the input.
	//   - A token of type Token::Error is returned if the input is invalid or if
	//     there was an error reading it.
	std::unique_ptr<Token> getNextToken();

	// Print an error message augmented with the current line number.
	void reportError(const char *msg, ...);

	// Returns true iff the lexer has attempted to read beyond the end of
	// the input yet.
	bool reachedEndOfFile() const { return InputStream->eof(); }
};

} // End garter namespace

#endif /* _GARTER_LEXER_H_ */
