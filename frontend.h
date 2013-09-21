#include <stddef.h>

struct ast_node {
	enum {
		X
	} type;
	size_t num_children;
	struct ast_node **children;
};

extern struct ast_node *
build_ast(const char *filename);
