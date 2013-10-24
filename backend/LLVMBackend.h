#ifndef _GARTER_LLVM_BACKEND_H_
#define _GARTER_LLVM_BACKEND_H_

#include <backend/Backend.h>
#include <memory>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

namespace garter {

class FunctionDefinitionAST;
class LLVMCodeGeneratorVisitor;

class LLVMBackend : public Backend {

	llvm::LLVMContext Ctx;
	llvm::Module Mod;
	llvm::IRBuilder<> Builder;
	llvm::IntegerType *Int32Ty;

	llvm::Function *generateFunctionPrototype(const FunctionDefinitionAST & func);
	llvm::Function *generateFunctionBodyCode(const FunctionDefinitionAST & func);

	friend class LLVMCodeGeneratorVisitor;

public:
	LLVMBackend(std::shared_ptr<ProgramAST> ast,
		    const char *module_name)
		: Backend(ast, module_name),
		  Ctx(),
		  Mod(module_name, Ctx),
		  Builder(Ctx),
		  Int32Ty(Builder.getInt32Ty())
	{
	}

	int codeGen(const char *outfile);
};

} // End garter namespace

#endif /* _GARTER_LLVM_BACKEND_H_ */
