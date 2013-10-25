#include <frontend/Parser.h>
#include <backend/LLVMBackend.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
	std::istream *is;
	std::ifstream infile;
	if (argc > 1) {
		infile.open(argv[1]);
		if (infile.fail())
			std::cerr << "Can't open " << argv[1] << ": " << strerror(errno) << std::endl;
		is = &infile;
	} else {
		is = &std::cin;
	}
	garter::Parser parser(*is);
	garter::LLVMBackend backend;
	std::unique_ptr<garter::ASTBase> top_level_item;

	while ((top_level_item = parser.parseTopLevelItem()) != nullptr)
		backend.executeTopLevelItem(std::move(top_level_item));

	if (!parser.reachedEndOfFile())
		return 1;

	return 0;
}
