#include <frontend/Lexer.h>

namespace garter {

class ASTNode {
};

class Parser {
private:
	Lexer Lexer;
	static const char * const Tag;

public:
	explicit Parser(llvm::MemoryBuffer *buffer) :
		Lexer(buffer)
	{
	}

	~Parser(){}

	ASTNode *BuildAST();
};

} // End garter namespace
