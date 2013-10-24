#include <frontend/Parser.h>
#include <backend/LLVMBackend.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <iostream>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>
#include <llvm/ADT/OwningPtr.h>

using namespace garter;

static void usage()
{
	const char *usage_str =
"Usage:  garterc FILE\n"
"Ahead of time compiler for the garter language; compiles the source\n"
"FILE into an executable.\n"
"\n"
"  -o OUTFILE      Use OUTFILE as name of executable (default: basename\n"
"                  of FILE).\n"
"\n"
	;

	fputs(usage_str, stderr);
}

static int
compile_file(const char *input_file, const char *output_file)
{
	const char *basename;

	llvm::error_code ec;
	llvm::OwningPtr<llvm::MemoryBuffer> input_buffer;

	ec = llvm::MemoryBuffer::getFile(input_file, input_buffer);
	if (ec) {
		std::cerr << "garterc ERROR: " << ec.message() << std::endl;
		return 1;
	}

	Parser parser(input_buffer->getBufferStart());

	std::shared_ptr<ProgramAST> ast = std::move(parser.buildAST());

	if (ast == nullptr) {
		std::cerr << "garterc ERROR: \""
			<< input_file << "\": ""AST not built" << std::endl;
		return 1;
	}

	basename = strrchr(input_file, '/');
	if (basename && *(basename + 1) != '\0')
		basename++;
	else
		basename = input_file;

	LLVMBackend backend(ast, basename);

	return backend.codeGen(output_file);
}

int main(int argc, char **argv)
{
	int c;
	char *input_file;
	char *output_file = NULL;

	while ((c = getopt(argc, argv, "o:h")) != -1) {
		switch (c) {
		case 'o':
			output_file = optarg;
			break;
		case 'h':
			usage();
			return 0;
		default:
			usage();
			return 2;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 1) {
		usage();
		return 2;
	}
	input_file = argv[0];
	if (output_file == NULL) {
		char *dot, *slash;

		output_file = strdupa(input_file);
		dot = strrchr(output_file, '.');
		slash = strrchr(output_file, '/');
		if (dot == NULL || (slash != NULL && dot <= slash + 1)) {
			fputs("ERROR: must provide -o for this input file\n", stderr);
			return 1;
		}
		*dot = '\0';
	}
	return compile_file(input_file, output_file);
}
