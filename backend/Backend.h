#ifndef _GARTER_BACKEND_H_
#define _GARTER_BACKEND_H_

#include <memory>

namespace garter {

class ProgramAST;
class ASTBase;

class Backend {
public:

	virtual int compileProgramToObjectFile(const ProgramAST & program,
					       const char *out_filename) = 0;
	virtual int executeTopLevelItem(const ASTBase & top_level_item) = 0;
};

} // End garter namespace

#endif /* _GARTER_BACKEND_H_ */
