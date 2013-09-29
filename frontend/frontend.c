#include "frontend.h"
#include "parse_state.h"
#include "parse.h"
#include "scan.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

struct ast_node *
new_ast_node(enum ast_node_type type)
{
	struct ast_node *node;

	node = malloc(sizeof(struct ast_node));
	if (node == NULL)
		return NULL;

	node->type = type;
	INIT_LIST_HEAD(&node->children);
	INIT_LIST_HEAD(&node->sibling_list);
	node->parent = NULL;
	return node;
}

struct ast_node *
ast_node_append_child(struct ast_node *parent, struct ast_node *child)
{
	list_add_tail(&child->sibling_list, &node->children);
	child->parent = node;
	return parent;
}

struct ast_node *
new_ast_node_with_child(enum ast_node_type type, struct ast_node *child)
{
	struct ast_node *node;

	node = new_ast_node(type);
	if (node == NULL)
		return NULL;

	return ast_node_append_child(node, child);
}

void
free_ast(struct ast_node *tree)
{
}

struct ast_node *
build_ast(const char *filename)
{
	FILE *fp;
	struct parse_state state;

	state.ast = NULL;
	state.filename = filename;

	fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Can't open \"%s\": %s\n", filename, strerror(errno));
		return NULL;
	}

	if (yylex_init(&state.scanner)) {
		fprintf(stderr, "Can't initialize scanner: %s\n", strerror(errno));
		fclose(fp);
		return NULL;
	}

	yyset_in(fp, state.scanner);

	if (yyparse(&state)) {
		free_ast(state.ast);
		state.ast = NULL;
	}

	yylex_destroy(state.scanner);
	fclose(fp);
	return state.ast;
}
