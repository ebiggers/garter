#ifndef _GARTER_LLVM_BACKEND_H_
#define _GARTER_LLVM_BACKEND_H_

#include <backend/Backend.h>
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

namespace llvm {
	class ExecutionEngine;
	class Module;
};

namespace garter {

class FunctionDefinitionAST;
class LLVMCodeGeneratorVisitor;

// Implementation of a garter Backend that uses LLVM for code generation.
// It additionally offers the function compileProgramToLLVMIR() for creating a
// LLVM IR file instead of a native object file.
class LLVMBackend : public Backend {

	llvm::LLVMContext Ctx;
	llvm::Module *Mod;
	llvm::IRBuilder<> Builder;
	llvm::IntegerType *Int32Ty;
	llvm::ExecutionEngine *Engine;
	unsigned long StatementNumber;

	llvm::Function *generateFunctionPrototype(const FunctionDefinitionAST & func);
	llvm::Function *generateFunctionBodyCode(const FunctionDefinitionAST & func,
						 bool toplevel = false);
	bool generateProgramIR(const ProgramAST & program);
	bool compileProgram(const ProgramAST & program,
			    const char *out_filename, bool obj_output);

	friend class LLVMCodeGeneratorVisitor;


public:
	LLVMBackend();
	~LLVMBackend();

	bool compileProgramToObjectFile(const ProgramAST & program,
					const char *out_filename)
	{
		return compileProgram(program, out_filename, true);
	}
	bool compileProgramToLLVMIR(const ProgramAST & program,
				    const char *out_filename)
	{
		return compileProgram(program, out_filename, false);
	}
	bool executeTopLevelItem(std::shared_ptr<ASTBase> top_level_item);
};

} // End garter namespace

#endif /* _GARTER_LLVM_BACKEND_H_ */
