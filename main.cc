#include <frontend/Parser.h>
#include <backend/LLVMBackend.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/system_error.h>

using namespace garter;

static llvm::SmallString<128> GarterRuntimeDir;

static llvm::cl::list<std::string>
InputFiles(llvm::cl::Positional, llvm::cl::OneOrMore, llvm::cl::desc("<input sources>"));

static llvm::cl::opt<std::string>
OutputFile("o", llvm::cl::desc("Output filename"), llvm::cl::value_desc("filename"));

static llvm::cl::opt<bool>
CompileOnly("c", llvm::cl::desc("Compile only (don't link)"));

static llvm::cl::opt<bool>
LLVMIROnly("l", llvm::cl::desc("Generate LLVM IR instead of a native object file (implies no linking)"));

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

static int
linkObjectFiles(const std::vector<llvm::SmallString<100>> & obj_files,
		const std::string & output)
{
	int ret;

	std::vector<const char *> argv;
	argv.push_back("clang");
	for (auto obj_file : obj_files)
		argv.push_back(obj_file.c_str());

	llvm::error_code ec;
	for (llvm::sys::fs::directory_iterator dir(GarterRuntimeDir.str(), ec), dir_end;
	     dir != dir_end && !ec; dir.increment(ec))
	{
		const std::string & path = dir->path();
		const llvm::StringRef ext(llvm::sys::path::extension(path));
		if (ext == ".o")
			argv.push_back(strdupa(path.c_str()));
	}

	if (ec) {
		std::cerr << "ERROR: " << ec.message() << std::endl;
		exit(-1);
	}

	argv.push_back("-o");
	argv.push_back(output.c_str());
	argv.push_back(NULL);


	ret = llvm::sys::Program::ExecuteAndWait(llvm::sys::Path("/usr/bin/clang"),
						 argv.data());
	if (ret == -1 || ret == -2)
		std::cerr << "Failed to run linker" << std::endl;
	return ret;
}

int main(int argc, char **argv)
{
	int ret;

	const char *ExecutablePath = realpath(argv[0], NULL);
	if (ExecutablePath != nullptr) {
		GarterRuntimeDir = llvm::StringRef(ExecutablePath);
		llvm::sys::path::remove_filename(GarterRuntimeDir);
		llvm::sys::path::append(GarterRuntimeDir, "runtime");
	} else {
		GarterRuntimeDir = llvm::StringRef("/usr/lib/garter");
	}

	llvm::cl::ParseCommandLineOptions(argc, argv, "garter compiler\n");

	bool do_link = !CompileOnly && !LLVMIROnly;

	if (OutputFile.length() > 0) {
		if (InputFiles.size() > 1 && !do_link) {
			std::cerr << "ERROR: cannot combine -o with either -l "
				"or -c using multiple input files" << std::endl;
			return 2;
		}
	}

	std::vector<llvm::SmallString<100>> output_files(InputFiles.size());
	for (size_t i = 0; i < InputFiles.size(); i++) {
		const std::string & input_file = InputFiles[i];
		const char *extension;

		if (llvm::sys::path::extension(input_file) == ".o") {
			output_files[i] = input_file;
			continue;
		}

		if (OutputFile.length() > 0 && !do_link) {
			output_files[i] = OutputFile;
		} else {
			output_files[i] = input_file;
			if (LLVMIROnly)
				extension = ".ll";
			else
				extension = ".o";
			llvm::sys::path::replace_extension(output_files[i], extension);
		}
		ret = compileFile(input_file.c_str(), output_files[i].c_str());
		if (ret)
			return ret;
	}

	if (do_link) {
		std::string exe_file;

		if (OutputFile.length() > 0)
			exe_file = OutputFile;
		else
			exe_file = "a.out";

		ret = linkObjectFiles(output_files, exe_file);
		if (ret)
			return ret;
	}

	return 0;
}
