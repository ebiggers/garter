#include <stdio.h>
#include "frontend.h"

int main(int argc, char **argv)
{
	struct ast_node *ast;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return 2;
	}

	ast = build_ast(argv[1]);
	if (ast == NULL)
		return 1;

	return 0;
}
