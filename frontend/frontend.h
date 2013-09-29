#pragma once

#include <stddef.h>
#include "list.h"

enum ast_node_type {
	AST_PRINT_STMT,
	AST_RETURN_STMT,
	AST_TARGET,
};

struct ast_node {
	enum ast_node_type type;
	struct list_head children;
	struct list_head sibling_node;
	struct ast_node *parent;
};


struct ast_node *
new_ast_node(enum ast_node_type type);

struct ast_node *
new_ast_node_with_children(enum ast_node_type type, struct ast_node *child1);

struct ast_node *
ast_node_append_child(struct ast_node *parent, struct ast_node *child);

struct ast_node *
build_ast(const char *filename);
