#include <frontend/Parser.h>
#include <iostream>
#include <algorithm>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <stdlib.h>
#include <sstream>
#include <ctype.h>

using namespace garter;

static bool ASTStringsEqual(const char *str1, const char *str2)
{
	for (;;) {
		while (isspace(*str1))
			str1++;
		while (isspace(*str2))
			str2++;
		while (*str1 == ',' && *str2 != ',')
			str1++;
		while (*str2 == ',' && *str1 != ',')
			str2++;
		if (*str1 == '\0' || *str2 == '\0') {
			if (*str1 != '\0' || *str2 != '\0')
				return false;
			return true;
		}
		if (*str1++ != *str2++)
			return false;
	}
}

static void doParserTest(const std::string & src_file_path,
			 const std::string & tree_file_path)
{
	llvm::error_code ec;
	llvm::OwningPtr<llvm::MemoryBuffer> src_buffer;
	llvm::OwningPtr<llvm::MemoryBuffer> tree_buffer;

	ec = llvm::MemoryBuffer::getFile(src_file_path, src_buffer);
	if (ec) {
		std::cerr << "TestParser ERROR: " << ec.message() << std::endl;
		exit(-1);
	}

	ec = llvm::MemoryBuffer::getFile(tree_file_path, tree_buffer);
	if (ec) {
		std::cerr << "TestParser ERROR: " << ec.message() << std::endl;
		exit(-1);
	}

	Parser parser(std::shared_ptr<llvm::MemoryBuffer>(src_buffer.take()));

	std::unique_ptr<ProgramAST> ast(parser.buildAST());

	if (ast == nullptr) {
		std::cerr << "TestParser ERROR: \"" << src_file_path
			<< " \": AST not built" << std::endl;
		exit(-1);
	}

	std::ostringstream os;
	os << *ast;

	if (!ASTStringsEqual(tree_buffer->getBufferStart(), os.str().c_str())) {
		std::cerr << "TestParser ERROR: Got\n";
		std::cerr << os.str() << "\n";
		std::cerr << "but expected:\n";
		std::cerr << tree_buffer->getBufferStart() << std::endl;
		exit(-1);
	}
}

int main()
{
	llvm::error_code ec;
	const char *dirpath = "test/ParserTests";

	std::vector<std::string> src_file_paths;
	std::vector<std::string> tree_file_paths;
	for (llvm::sys::fs::directory_iterator dir(dirpath, ec), dir_end;
	     dir != dir_end; dir.increment(ec))
	{
		const std::string & path = dir->path();
		const llvm::StringRef ext(llvm::sys::path::extension(path));
		if (ext == ".garter")
			src_file_paths.push_back(path);
		else if (ext == ".tree")
			tree_file_paths.push_back(path);
	}
	if (ec) {
		std::cerr << "TestParser ERROR: " << ec.message() << std::endl;
		exit(-1);
	}
	std::sort(src_file_paths.begin(), src_file_paths.end());
	std::sort(tree_file_paths.begin(), tree_file_paths.end());

	if (src_file_paths.size() != tree_file_paths.size()) {
		std::cerr << "TestParser ERROR: Wrong number of test files in %s"
			<< dirpath << std::endl;
		exit(-1);
	}

	for (size_t i = 0; i < src_file_paths.size(); i++) {
		if (llvm::sys::path::stem(src_file_paths[i]) !=
		    llvm::sys::path::stem(tree_file_paths[i]))
		{
			std::cerr << "TestParser ERROR: Wrong test filenames in %s"
				<< dirpath << std::endl;
			exit(-1);
		}
		doParserTest(src_file_paths[i], tree_file_paths[i]);
	}

	printf("=======================================\n");
	printf("  TestParser:  All tests passed!\n");
	printf("=======================================\n");
	return 0;
}
