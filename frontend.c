#include "frontend.h"
#include "parse.h"
#include "parse_state.h"
#include "scan.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

struct ast_node *
build_ast(const char *filename)
{
	FILE *fp;
	struct parse_state state = {.ast = NULL};

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

	yyparse(&state);

	yylex_destroy(state.scanner);
	fclose(fp);
	return state.ast;
}
