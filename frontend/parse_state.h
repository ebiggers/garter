#pragma once

struct ast_node;

struct parse_state {
	struct ast_node *ast;
	void *scanner;
	const char *filename;
};
