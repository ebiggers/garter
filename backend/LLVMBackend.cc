#include <backend/LLVMBackend.h>
#include <frontend/Parser.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/Host.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/PassManager.h>
#include <iostream>

using namespace garter;
using namespace llvm;

LLVMBackend::LLVMBackend()
		: Ctx(),
		  Mod("", Ctx),
		  Builder(Ctx),
		  Int32Ty(Builder.getInt32Ty()),
		  Engine(nullptr)
{
}

LLVMBackend::~LLVMBackend()
{
	delete Engine;
}

// Given the AST node for a function definition, create and return the
// llvm::Function corresponding to the function prototype.  Returns nullptr if
// an identically-named function was already defined.
Function *LLVMBackend::generateFunctionPrototype(const FunctionDefinitionAST & func,
						 Function::LinkageTypes linkage)
{
	// Get function type
	FunctionType *funcTy;
	{
		std::vector<Type*> param_types(func.Parameters.size(), Int32Ty);
		funcTy = FunctionType::get(Int32Ty, param_types, false);
	}

	// Create the function
	Function *f = Function::Create(funcTy, linkage, func.Name, &Mod);

	// Check for multiple definition
	if (f->getName() != func.Name) {
		std::cerr << "LLVMBackend: ERROR: Multiple definitions of "
			  << func.Name << std::endl;
		return nullptr;
	}

	// Set parameter names
	{
		size_t i = 0;
		for (Function::arg_iterator argptr = f->arg_begin();
		     argptr != f->arg_end(); i++, argptr++)
		{
			argptr->setName(func.Parameters[i]);
		}
	}
	return f;
}

namespace garter {

// StatementAST and ExpressionAST visitor for LLVM IR generation
class LLVMCodeGeneratorVisitor : public StatementASTVisitor,
				 public ExpressionASTVisitor {
private:
	LLVMBackend & Backend;

	// Current function for which IR is being generated
	Function * CurrentFunction;

	// Mapping from variable names to LLVM IR values in this function
	std::map<std::string, Value*> & NamedValues;

	// Used to return the LLVM IR value generated from an expression
	Value *ExpressionValue;
	Value *ExpressionPointer;

	// Used to return whether LLVM IR was successfully generated for a
	// statement
	bool StatementSuccessful;

	Value *isZeroOrNotZero(Value *val, bool is_zero);
	Value *isZero(Value *val);
	Value *isNotZero(Value *val);
public:
	LLVMCodeGeneratorVisitor(LLVMBackend & backend, Function * f,
				 std::map<std::string, Value*> & named_values)
		: Backend(backend), CurrentFunction(f),
		  NamedValues(named_values), ExpressionValue(nullptr)
	{ }

	void visit(AssignmentStatementAST &);
	void visit(ExpressionStatementAST &);
	void visit(IfStatementAST &);
	void visit(PassStatementAST &);
	void visit(PrintStatementAST &);
	void visit(ReturnStatementAST &);
	void visit(WhileStatementAST &);

	void visit(BinaryExpressionAST &);
	void visit(CallExpressionAST &);
	void visit(NumberExpressionAST &);
	void visit(UnaryExpressionAST &);
	void visit(VariableExpressionAST &);

	bool getStatementSuccessful() const { return StatementSuccessful; }
};
} // end garter namespace

// Given a LLVM IR integer value @val, generate LLVM IR in the current function
// testing whether the value is zero or not.  If @is_zero, returns a LLVM IR
// value that represents 1 if the input value is 0, or 0 if the input value is
// nonzero.  If !@in_zero, the return values are inverted.
Value *LLVMCodeGeneratorVisitor::isZeroOrNotZero(Value *val, bool is_zero)
{
	BasicBlock *zerobb, *nonzerobb;
	BasicBlock *contbb;
	Value *zero, *cmpresult;
	Value *zeroresult, *nonzeroresult;
	PHINode *phi;
	
	zerobb = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);
	nonzerobb = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);
	contbb = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);

	zero = Backend.Builder.getInt32(0);
	cmpresult = Backend.Builder.CreateICmpEQ(zero, val);
	Backend.Builder.CreateCondBr(cmpresult, zerobb, nonzerobb);

	Backend.Builder.SetInsertPoint(zerobb);
	zeroresult = Backend.Builder.getInt32(is_zero ? 1 : 0);
	Backend.Builder.CreateBr(contbb);

	Backend.Builder.SetInsertPoint(nonzerobb);
	nonzeroresult = Backend.Builder.getInt32(is_zero ? 0 : 1);
	Backend.Builder.CreateBr(contbb);

	Backend.Builder.SetInsertPoint(contbb);
	phi = Backend.Builder.CreatePHI(Backend.Int32Ty, 2);
	phi->addIncoming(zeroresult, zerobb);
	phi->addIncoming(nonzeroresult, nonzerobb);
	return phi;
}

Value *LLVMCodeGeneratorVisitor::isNotZero(Value *val)
{
	return isZeroOrNotZero(val, false);
}

Value *LLVMCodeGeneratorVisitor::isZero(Value *val)
{
	return isZeroOrNotZero(val, true);
}

// Generate LLVM IR in the current function for a binary expression.  The
// resulting pointer to the llvm::Value is returned in this->ExpressionValue.
void LLVMCodeGeneratorVisitor::visit(BinaryExpressionAST & expr)
{
	expr.LHS->acceptVisitor(*this);
	if (ExpressionValue == nullptr)
		return;
	Value *lhs_value = ExpressionValue;

	expr.RHS->acceptVisitor(*this);
	if (ExpressionValue == nullptr)
		return;
	Value *rhs_value = ExpressionValue;

	ExpressionValue = nullptr;
	switch (expr.Op) {
	case BinaryExpressionAST::None:
		// Shouldn't get here.
		assert(0);
		break;
	case BinaryExpressionAST::Or:
		ExpressionValue = Backend.Builder.CreateOr(isNotZero(lhs_value),
							   isNotZero(rhs_value));
		break;
	case BinaryExpressionAST::And:
		ExpressionValue = Backend.Builder.CreateAnd(isNotZero(lhs_value),
							    isNotZero(rhs_value));
		break;
	case BinaryExpressionAST::LessThan:
		ExpressionValue = Backend.Builder.CreateICmpSLT(lhs_value,
								rhs_value);
		break;
	case BinaryExpressionAST::GreaterThan:
		ExpressionValue = Backend.Builder.CreateICmpSGT(lhs_value,
								rhs_value);
		break;
	case BinaryExpressionAST::LessThanOrEqualTo:
		ExpressionValue = Backend.Builder.CreateICmpSLE(lhs_value,
								rhs_value);
		break;
	case BinaryExpressionAST::GreaterThanOrEqualTo:
		ExpressionValue = Backend.Builder.CreateICmpSGE(lhs_value,
								rhs_value);
		break;
	case BinaryExpressionAST::EqualTo:
		ExpressionValue = Backend.Builder.CreateICmpEQ(lhs_value,
							       rhs_value);
		break;
	case BinaryExpressionAST::NotEqualTo:
		ExpressionValue = Backend.Builder.CreateICmpNE(lhs_value,
							       rhs_value);
		break;
	case BinaryExpressionAST::Add:
		ExpressionValue = Backend.Builder.CreateAdd(lhs_value,
							    rhs_value);
		break;
	case BinaryExpressionAST::Subtract:
		ExpressionValue = Backend.Builder.CreateSub(lhs_value,
							    rhs_value);
		break;
	case BinaryExpressionAST::Multiply:
		ExpressionValue = Backend.Builder.CreateMul(lhs_value,
							    rhs_value);
		break;
	case BinaryExpressionAST::Divide:
		ExpressionValue = Backend.Builder.CreateSDiv(lhs_value,
							     rhs_value);
		break;
	case BinaryExpressionAST::Modulo:
		ExpressionValue = Backend.Builder.CreateSRem(lhs_value,
							     rhs_value);
		break;
	case BinaryExpressionAST::Exponentiate:
		/* TODO */
		assert(0);
		break;
	case BinaryExpressionAST::In:
		/* TODO */
		assert(0);
		break;
	case BinaryExpressionAST::NotIn:
		/* TODO */
		assert(0);
		break;
	}
	if (ExpressionValue != nullptr)
		ExpressionValue = Backend.Builder.CreateZExt(ExpressionValue, Backend.Int32Ty);
}

// Generate LLVM IR in the current function for a function call.  The resulting
// pointer to the llvm::Value representing the return value of the called
// function is returned in this->ExpressionValue.
void LLVMCodeGeneratorVisitor::visit(CallExpressionAST & expr)
{
	Function *callee = Backend.Mod.getFunction(expr.Callee);

	// Make sure the called function is actually declared
	if (callee == nullptr) {
		std::cerr << "LLVMBackend: ERROR: Unknown function "
			  << expr.Callee << std::endl;
		ExpressionValue = nullptr;
		return;
	}

	// Make sure the function is called with the correct number of arguments
	if (callee->arg_size() != expr.Arguments.size()) {
		std::cerr << "LLVMBackend: ERROR: Wrong number of arguments to "
			  << expr.Callee << std::endl;
		ExpressionValue = nullptr;
		return;
	}

	// Build a vector of llvm::Value pointers representing the function
	// arguments.  Note that this may involve recursive IR generation for
	// calculating the values of the argument expressions.
	std::vector<Value*> args;
	for (auto exprptr : expr.Arguments) {
		exprptr->acceptVisitor(*this);
		if (ExpressionValue == nullptr)
			return;
		args.push_back(ExpressionValue);
	}
	ExpressionValue = Backend.Builder.CreateCall(callee, args);
}

// Generate LLVM IR in the current function for a numeric literal.
void LLVMCodeGeneratorVisitor::visit(NumberExpressionAST & expr)
{
	ExpressionValue = Backend.Builder.getInt32(expr.Number);
}

// Generate LLVM IR in the current function for a unary expression.  The
// resulting pointer to the llvm::Value is returned in this->ExpressionValue.
void LLVMCodeGeneratorVisitor::visit(UnaryExpressionAST & expr)
{
	expr.Expression->acceptVisitor(*this);
	if (ExpressionValue == nullptr)
		return;

	switch (expr.Op) {
	case UnaryExpressionAST::Minus:
		{
			Value *zero = Backend.Builder.getInt32(0);
			ExpressionValue = Backend.Builder.CreateSub(zero, ExpressionValue);
		}
		break;
	case UnaryExpressionAST::Plus:
		// Unary plus does nothing
		return;
	case UnaryExpressionAST::Not:
		// unary not:  0 -> 1, nonzero -> 0
		ExpressionValue =
			Backend.Builder.CreateZExt(isZero(ExpressionValue),
						   Backend.Int32Ty);
		break;
	}
}

// Generate LLVM IR in the current function for a variable expression.  The
// resulting pointer to the llvm::Value representing the variable is returned in
// this->ExpressionValue.
void LLVMCodeGeneratorVisitor::visit(VariableExpressionAST & expr)
{
	Value *var_ptr = NamedValues[expr.Name];
	Value *var_value;
	if (var_ptr == nullptr) {
		Value *zero = Backend.Builder.getInt32(0);

		// Variable didn't already exist in the current function; create it.
		var_ptr = Backend.Builder.CreateAlloca(Backend.Int32Ty,
						       0, expr.Name);
		NamedValues[expr.Name] = var_ptr;
		Backend.Builder.CreateStore(zero, var_ptr);
		var_value = zero;
	} else {
		// Variable already existed in the current function.
		var_value = Backend.Builder.CreateLoad(var_ptr);
	}
	ExpressionValue = var_value;
	ExpressionPointer = var_ptr;
}

// Generate LLVM IR in the current function for an assignment statement.
void LLVMCodeGeneratorVisitor::visit(AssignmentStatementAST & stmt)
{
	StatementSuccessful = false;

	stmt.Variable->acceptVisitor(*this);
	if (ExpressionValue == nullptr)
		return;
	Value *var_ptr = ExpressionPointer;

	stmt.Expression->acceptVisitor(*this);
	if (ExpressionValue == nullptr)
		return;

	if (Backend.Builder.CreateStore(ExpressionValue, var_ptr) == nullptr)
		return;

	StatementSuccessful = true;
}

// Generate LLVM IR in the current function for an expression statement.
void LLVMCodeGeneratorVisitor::visit(ExpressionStatementAST & stmt)
{
	StatementSuccessful = false;

	stmt.Expression->acceptVisitor(*this);
	if (ExpressionValue == nullptr)
		return;
	// The value of the expression is ignored in this type of statement.

	StatementSuccessful = true;
}

void LLVMCodeGeneratorVisitor::visit(IfStatementAST & stmt)
{
	StatementSuccessful = false;

	BasicBlock *body, *nextbb, *contbb;

	// contbb - basic block executed after the entire if statement
	contbb = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);

	size_t i = 0;
	do {
		// body - basic block beginning the body of the current if or
		// elif clause (tested condition true)
		body = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);

		// nextbb - basic block executed otherwise (tested condition
		// false)
		nextbb = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);

		// Generate LLVM IR for condition test
		if (i == 0)
			stmt.Condition->acceptVisitor(*this);
		else
			stmt.ElifClauses[i - 1]->Condition->acceptVisitor(*this);
		if (ExpressionValue == nullptr)
			return;
		Value *cond = isNotZero(ExpressionValue);

		// Chose next basic block based on condition
		Backend.Builder.CreateCondBr(cond, body, nextbb);

		// Generate LLVM IR for the if or elif body
		Backend.Builder.SetInsertPoint(body);

		const std::vector<std::shared_ptr<StatementAST>> & body =
			(i == 0) ? stmt.Body : stmt.ElifClauses[i - 1]->Body;
		for (auto stmtptr : body) {
			stmtptr->acceptVisitor(*this);
			if (!StatementSuccessful)
				return;
		}
		// Branch to end of if statement after if or elif body
		Backend.Builder.CreateBr(contbb);

		// Now set the insertion point to basic block where control flow
		// goes if the 'if' or 'elif' condition is false
		Backend.Builder.SetInsertPoint(nextbb);

	} while (i++ != stmt.ElifClauses.size());

	// Generate LLVM IR for the else body (may be empty)
	for (auto stmtptr : stmt.ElseBody) {
		stmtptr->acceptVisitor(*this);
		if (!StatementSuccessful)
			return;
	}
	// Branch to end of if statement after (possibly empty) else body
	Backend.Builder.CreateBr(contbb);
	Backend.Builder.SetInsertPoint(contbb);
}

// Generate LLVM IR in the current function for a pass statement.
void LLVMCodeGeneratorVisitor::visit(PassStatementAST & stmt __attribute__((unused)))
{
	// Nothing to do.
	StatementSuccessful = true;
}

// Generate LLVM IR in the current function for a print statement.
void LLVMCodeGeneratorVisitor::visit(PrintStatementAST & stmt)
{
	StatementSuccessful = false;

	// Retrieve print function (in runtime library)
	FunctionType *funcTy = FunctionType::get(Backend.Int32Ty, Backend.Int32Ty, true);
	Constant *print = Backend.Mod.getOrInsertFunction("__garter_print", funcTy);
	assert(print != nullptr);

	// Build a vector of llvm::Value pointers representing the function
	// arguments.  Note that this may involve recursive IR generation for
	// calculating the values of the argument expressions.
	std::vector<Value*> args;
	args.push_back(Backend.Builder.getInt32(stmt.Arguments.size()));
	for (auto exprptr : stmt.Arguments) {
		exprptr->acceptVisitor(*this);
		if (ExpressionValue == nullptr)
			return;
		args.push_back(ExpressionValue);
	}
	Backend.Builder.CreateCall(print, args);
	StatementSuccessful = true;
}

// Generate LLVM IR in the current function for a return statement.
void LLVMCodeGeneratorVisitor::visit(ReturnStatementAST & stmt)
{
	StatementSuccessful = false;

	stmt.Expression->acceptVisitor(*this);
	if (ExpressionValue == nullptr)
		return;

	Backend.Builder.CreateRet(ExpressionValue);


	BasicBlock *newbb = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);
	Backend.Builder.SetInsertPoint(newbb);

	StatementSuccessful = true;
}

// Generate LLVM IR in the current function for a while statement.
void LLVMCodeGeneratorVisitor::visit(WhileStatementAST & stmt)
{
	StatementSuccessful = false;

	// Basic block for testing while loop condition
	BasicBlock *condbb = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);

	// Basic black for while loop body
	BasicBlock *bodybb = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);

	// Basic block for continuing after finishing while loop
	BasicBlock *contbb = BasicBlock::Create(Backend.Ctx, "", CurrentFunction);

	// Branch from current position to while loop condition
	Backend.Builder.CreateBr(condbb);

	// Generate IR for while loop condition
	Backend.Builder.SetInsertPoint(condbb);
	stmt.Condition->acceptVisitor(*this);
	if (ExpressionValue == nullptr)
		return;
	Value *cond = isNotZero(ExpressionValue);
	// Continue or break loop based on condition
	Backend.Builder.CreateCondBr(cond, bodybb, contbb);

	// Generate IR for loop body
	Backend.Builder.SetInsertPoint(bodybb);
	for (auto stmtptr : stmt.Body) {
		stmtptr->acceptVisitor(*this);
		if (!StatementSuccessful)
			return;
	}
	// At end of body, jump to condition test
	Backend.Builder.CreateBr(condbb);

	// Finally, set the insertion point to the basic block after the while
	// statement.
	Backend.Builder.SetInsertPoint(contbb);

	StatementSuccessful = true;
}

// Generate LLVM IR for a function body.
//
// It is assumed that the function has already had a prototype generated in the
// current module.
//
// Returns nullptr on failure; otherwise the llvm::Function pointer
Function *LLVMBackend::generateFunctionBodyCode(const FunctionDefinitionAST & func)
{
	Function * f = Mod.getFunction(func.Name);

	assert(f != nullptr);

	// Create entry basic block
	BasicBlock *bb = BasicBlock::Create(Ctx, "", f);
	Builder.SetInsertPoint(bb);

	std::map<std::string, Value *> named_values;

	// Store function parameters into alloca slots
	{
		size_t i = 0;
		for (Function::arg_iterator argptr = f->arg_begin();
		     argptr != f->arg_end(); i++, argptr++)
		{
			AllocaInst *a = Builder.CreateAlloca(Int32Ty, 0, func.Parameters[i]);
			Builder.CreateStore(argptr, a);
			named_values[func.Parameters[i]] = a;
		}
	}

	// Generate IR for function body statements
	LLVMCodeGeneratorVisitor gen(*this, f, named_values);
	for (auto stmtptr : func.Body) {
		stmtptr->acceptVisitor(gen);
		if (!gen.getStatementSuccessful())
			return nullptr;
	}
	Builder.CreateRet(Builder.getInt32(0));

	assert (llvm::verifyFunction (*f));

	return f;
}

int LLVMBackend::generateProgramCode(const ProgramAST & program)
{
	// Generate prototypes for all functions
	for (auto itemptr : program.TopLevelItems) {
		auto func = std::dynamic_pointer_cast<FunctionDefinitionAST>(itemptr);
		if (func == nullptr)
			continue;

		if (nullptr == generateFunctionPrototype(*func, Function::InternalLinkage))
			return 1;
	}

	// Treat toplevel statements as anonymous function
	std::vector<std::shared_ptr<StatementAST>> main_body;
	for (auto itemptr : program.TopLevelItems) {
		auto stmt = std::dynamic_pointer_cast<StatementAST>(itemptr);
		if (stmt == nullptr)
			continue;
		main_body.push_back(stmt);
	}

	std::unique_ptr<FunctionDefinitionAST> main_ast (
		new FunctionDefinitionAST("__garter_main", std::vector<std::string>(),
					  main_body));
	if (nullptr == generateFunctionPrototype(*main_ast, Function::ExternalLinkage))
		return 1;

	// Generate code for all functions, plus the anonymous function
	// containing the toplevel statements
	for (auto itemptr : program.TopLevelItems) {
		auto func = std::dynamic_pointer_cast<FunctionDefinitionAST>(itemptr);
		if (func == nullptr)
			continue;

		if (nullptr == generateFunctionBodyCode(*func))
			return 1;
	}
	if (nullptr == generateFunctionBodyCode(*main_ast))
		return 1;

	return 0;
}

int LLVMBackend::compileProgram(const ProgramAST & program,
				const char *out_filename, bool obj_output)
{
	int ret = generateProgramCode(program);
	if (ret)
		return ret;

	std::string err_str;

	/* XXX: llvm::sys::fs::F_Binary flag should be used to open the output
	 * file, but it was not available in LLVM version being tested.  */
	tool_output_file os(out_filename, err_str);
	if (err_str.length() != 0) {
		std::cerr << "LLVMBackend: " << err_str << std::endl;
		return 1;
	}

	if (obj_output) {
		std::string err_str;
		std::string triple;
		std::string cpu;
		std::string features;
		TargetOptions options;
		const Target *target;
		std::unique_ptr<TargetMachine> mach;
		PassManager mgr;
		formatted_raw_ostream out(os.os());
		
		llvm::InitializeAllTargets();
		llvm::InitializeAllTargetMCs();
		llvm::InitializeAllAsmPrinters();

		triple = sys::getDefaultTargetTriple();
		cpu = sys::getHostCPUName();

		target = TargetRegistry::lookupTarget(triple, err_str);
		if (target == nullptr) {
			std::cerr << "LLVMBackend: " << err_str << std::endl;
			return 1;
		}

		mach.reset(target->createTargetMachine(triple, cpu, features, options));
		if (mach == nullptr) {
			std::cerr << "LLVMBackend: couldn't create TargetMachine" << std::endl;
			return 1;
		}

		if (mach->addPassesToEmitFile(mgr, out,
					      TargetMachine::CGFT_ObjectFile,
					      false))
		{
			std::cerr << "LLVMBackend: couldn't add passes "
				"to create object file" << std::endl;
			return 1;
		}
		mgr.run(Mod);
	} else {
		Mod.print(os.os(), nullptr);
	}

	os.keep();

	return 0;
}

int LLVMBackend::executeTopLevelItem(const ASTBase & top_level_item)
{
	const FunctionDefinitionAST * func =
		dynamic_cast<const FunctionDefinitionAST *>(&top_level_item);
	const StatementAST * stmt =
		dynamic_cast<const StatementAST *>(&top_level_item);
	Function *f;

	if (stmt) {
		if (Engine == nullptr) {
			llvm::InitializeNativeTarget();
			Engine = EngineBuilder(&Mod).create();
		}
		std::shared_ptr<StatementAST> stmtptr(const_cast<StatementAST*>(stmt));
		FunctionDefinitionAST func("", {}, {stmtptr});

		f = generateFunctionPrototype(func, Function::InternalLinkage);
		if (f == nullptr)
			return 1;
		f = generateFunctionBodyCode(func);
		if (f == nullptr)
			return 1;

		Engine->runFunction(f, {});
	} else {
		f = generateFunctionPrototype(*func, Function::InternalLinkage);
		if (f == nullptr)
			return 1;
		f = generateFunctionBodyCode(*func);
		if (f == nullptr)
			return 1;
	}

	return 0;
}
