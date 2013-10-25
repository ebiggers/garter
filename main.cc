#include <frontend/Parser.h>
#include <backend/LLVMBackend.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/system_error.h>

using namespace garter;

static llvm::cl::opt<std::string>
InputFilename(llvm::cl::Positional, llvm::cl::Required, llvm::cl::desc("<input source>"));

static llvm::cl::opt<std::string>
OutputFilename("o", llvm::cl::desc("Output filename"), llvm::cl::value_desc("filename"));

static llvm::cl::opt<bool>
LLVMIROnly("l", llvm::cl::desc("Generate LLVM IR instead of a native object file"));

std::unique_ptr<ProgramAST>
parseFile(const char *input_file)
{
	llvm::error_code ec;
	llvm::OwningPtr<llvm::MemoryBuffer> input_buffer;

	ec = llvm::MemoryBuffer::getFile(input_file, input_buffer);
	if (ec) {
		std::cerr << "ERROR: \"" << input_file << "\": "
			<< ec.message() << std::endl;
		return nullptr;
	}

	Parser parser(input_buffer->getBufferStart());

	std::unique_ptr<ProgramAST> program = parser.parseProgram();

	if (program == nullptr)
		return nullptr;

	return program;
}

static int
compileFile(const char *input_file, const char *output_file)
{
	std::unique_ptr<ProgramAST> program = parseFile(input_file);
	if (program == nullptr) {
		std::cerr << "garterc: Compilation terminated." << std::endl;
		return 1;
	}

	LLVMBackend backend;

	if (LLVMIROnly)
		return backend.compileProgramToLLVMIR(*program, output_file);
	else
		return backend.compileProgramToObjectFile(*program, output_file);
}

int main(int argc, char **argv)
{
	llvm::cl::ParseCommandLineOptions(argc, argv, "garter compiler\n");

	if (OutputFilename == "") {
		const char * extension = (LLVMIROnly ? ".ll" : ".o");
		llvm::SmallString<255> output_filename(InputFilename);
		llvm::sys::path::replace_extension(output_filename, extension);
		OutputFilename = output_filename.str();
	}

	return compileFile(InputFilename.c_str(), OutputFilename.c_str());
}
