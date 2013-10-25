#ifndef _GARTER_LLVM_BACKEND_H_
#define _GARTER_LLVM_BACKEND_H_

#include <backend/Backend.h>
#include <memory>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

namespace llvm {
	class ExecutionEngine;
};

namespace garter {

class FunctionDefinitionAST;
class LLVMCodeGeneratorVisitor;

class LLVMBackend : public Backend {

	llvm::LLVMContext Ctx;
	llvm::Module *Mod;
	llvm::IRBuilder<> Builder;
	llvm::IntegerType *Int32Ty;
	llvm::ExecutionEngine *Engine;

	llvm::Function *generateFunctionPrototype(const FunctionDefinitionAST & func);
	llvm::Function *generateFunctionBodyCode(const FunctionDefinitionAST & func);
	int generateProgramCode(const ProgramAST & program);
	int compileProgram(const ProgramAST & program,
			   const char *out_filename, bool obj_output);

	friend class LLVMCodeGeneratorVisitor;


public:
	LLVMBackend();
	~LLVMBackend();

	int compileProgramToObjectFile(const ProgramAST & program,
				       const char *out_filename)
	{
		return compileProgram(program, out_filename, true);
	}
	int compileProgramToLLVMIR(const ProgramAST & program,
				   const char *out_filename)
	{
		return compileProgram(program, out_filename, false);
	}
	int executeTopLevelItem(std::shared_ptr<ASTBase> top_level_item);
};

} // End garter namespace

#endif /* _GARTER_LLVM_BACKEND_H_ */
