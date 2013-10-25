#ifndef _GARTER_BACKEND_H_
#define _GARTER_BACKEND_H_

#include <memory>

namespace garter {

class ProgramAST;
class ASTBase;

// Interface implemented by garter backends
class Backend {
public:

	// Given the AST representing a program, compile it to a native object
	// file.  Returns true if successful, otherwise false.
	virtual bool compileProgramToObjectFile(const ProgramAST & program,
						const char *out_filename) = 0;

	// Given the AST representing the next top-level statement in the
	// program, execute it using either an interpreter or a just-in-time
	// compiler.  Returns true if successful, otherwise false.
	virtual bool executeTopLevelItem(std::shared_ptr<ASTBase> top_level_item) = 0;
};

} // End garter namespace

#endif /* _GARTER_BACKEND_H_ */
