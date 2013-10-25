#include <frontend/Parser.h>
#include <backend/LLVMBackend.h>
#include <iostream>

int main()
{
	std::string line;
	garter::Parser parser(std::cin);

	garter::LLVMBackend backend;

	std::unique_ptr<garter::ASTBase> top_level_item;
	while ((top_level_item = parser.parseTopLevelItem()) != nullptr) {
		backend.executeTopLevelItem(std::move(top_level_item));
	}

	if (!parser.reachedEndOfFile())
		return 1;

	return 0;
}
