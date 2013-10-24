#ifndef _GARTER_BACKEND_H_
#define _GARTER_BACKEND_H_

#include <memory>

namespace garter {

class ProgramAST;

class Backend {
protected:
	std::shared_ptr<ProgramAST> AST;
	const char *ModuleName;

public:
	Backend(std::shared_ptr<ProgramAST> ast,
		const char *module_name)
		: AST(ast), ModuleName(module_name)
	{
	}

	virtual int codeGen(const char *outfile) = 0;
};

} // End garter namespace

#endif /* _GARTER_BACKEND_H_ */
