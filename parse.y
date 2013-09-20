%define api.pure full
%name-prefix "garter_"
%locations

%{
#define YYSTYPE int
#define YYPARSE_PARAM yyscan_t scanner
#define YYLEX_PARAM scanner
#include "parse.h"
#include "scan.h"
void yyerror(YYLTYPE *, const char *);
%}

%token TOK_AND
%token TOK_ASSIGNMENT
%token TOK_ASTERISK
%token TOK_COLON
%token TOK_COMMA
%token TOK_DEF
%token TOK_DOUBLE_ASTERISK
%token TOK_ELIF
%token TOK_ELSE
%token TOK_ENDEF
%token TOK_ENDFOR
%token TOK_ENDIF
%token TOK_ENDWHILE
%token TOK_EQ
%token TOK_FALSE
%token TOK_FOR
%token TOK_FORWARD_SLASH
%token TOK_GE
%token TOK_GT
%token TOK_IDENTIFIER
%token TOK_IF
%token TOK_IN
%token TOK_INTEGER
%token TOK_LE
%token TOK_LEFT_PAREN
%token TOK_LEFT_SQUARE_BRACKET
%token TOK_LT
%token TOK_MINUS
%token TOK_NEQ
%token TOK_NOT
%token TOK_OR
%token TOK_PASS
%token TOK_PERCENT
%token TOK_PLUS
%token TOK_PRINT
%token TOK_RETURN
%token TOK_RIGHT_PAREN
%token TOK_RIGHT_SQUARE_BRACKET
%token TOK_SEMICOLON
%token TOK_TRUE
%token TOK_WHILE

%%
/* Grammar rules and actions  */

program:  toplevel_items
       ;

toplevel_items:  toplevel_item toplevel_items { }
	       | { }
	       ;

toplevel_item:  stmt { }
	      | func_def { }
	      ;

stmt:  simple_stmt TOK_SEMICOLON { }
     | compound_stmt { }
     ;

stmt_seq:  stmt { }
	 | stmt stmt_seq {}
	 ;

simple_stmt:  TOK_PASS { }
	    | assignment_stmt { }
	    | print_stmt { }
	    | return_stmt { }
	    ;
 
assignment_stmt:  target TOK_ASSIGNMENT expr { }
	        ;

target:  TOK_IDENTIFIER { }
       | subscription { }
       ;

print_stmt:  TOK_PRINT expr_seq { }
	   ;

return_stmt:  TOK_RETURN expr { }
	    ;

compound_stmt:  if_stmt { }
	      | while_stmt { }
	      | for_stmt { }
	      ;

if_stmt:  TOK_IF expr TOK_COLON stmt_seq elif_clauses TOK_ENDIF { }
        | TOK_IF expr TOK_COLON stmt_seq elif_clauses TOK_ELSE TOK_COLON stmt_seq TOK_ENDIF { }
	;

elif_clauses:  TOK_ELIF expr TOK_COLON stmt_seq elif_clauses { }
	     | { }
	     ;

while_stmt:  TOK_WHILE expr TOK_COLON stmt_seq TOK_ENDWHILE { }
	   ;

for_stmt: TOK_FOR TOK_IDENTIFIER TOK_IN expr TOK_COLON stmt_seq TOK_ENDFOR { }
	 ;

expr:  or_expr { }
	   ;

or_expr:  and_expr { }
        | or_expr TOK_OR and_expr  { }
	;

and_expr:  not_expr { }
	 | and_expr TOK_AND not_expr { }
	 ;

not_expr:  comparison { }
	 | TOK_NOT not_expr { }
	 ;

comparison:  add_expr { }
	   | add_expr comp_operator add_expr { }
	   ;

comp_operator:  TOK_LT { }
	      | TOK_GT { }
	      | TOK_LE { }
	      | TOK_GE { }
	      | TOK_EQ { }
	      | TOK_NEQ { }
	      | TOK_IN { }
	      | TOK_NOT TOK_IN { }
	      ;

add_expr:  mult_expr { }
	 | add_expr TOK_PLUS mult_expr { }
	 | add_expr TOK_MINUS mult_expr { }
	 ;

mult_expr:  unary_expr { }
	  | mult_expr TOK_ASTERISK unary_expr { }
	  | mult_expr TOK_FORWARD_SLASH unary_expr { }
	  | mult_expr TOK_PERCENT unary_expr { }

unary_expr:  power_expr { }
	   | TOK_MINUS unary_expr { }
	   | TOK_PLUS unary_expr { }
	   ;

power_expr:  primary TOK_DOUBLE_ASTERISK unary_expr { }
	   | primary { }
	   ;

primary:  atom { }
        | subscription { }
	| call
	;

atom:  TOK_INTEGER { }
     | TOK_IDENTIFIER { }
     | paren_expr { }
     | list_expr { }
     ;

paren_expr:  TOK_LEFT_PAREN expr TOK_RIGHT_PAREN { }
	   ;

list_expr:  TOK_LEFT_SQUARE_BRACKET expr_seq TOK_RIGHT_SQUARE_BRACKET { }
	  ;

expr_seq:  nonempty_expr_seq { }
	 | { }
	 ;

nonempty_expr_seq:  expr { }
		  | expr TOK_COMMA nonempty_expr_seq { }
		  ;

subscription:  TOK_IDENTIFIER subscripts { }
	     ;

subscripts:  TOK_LEFT_SQUARE_BRACKET expr TOK_RIGHT_SQUARE_BRACKET { }
	   | TOK_LEFT_SQUARE_BRACKET expr TOK_RIGHT_SQUARE_BRACKET subscripts { }
	   ;

call:  TOK_IDENTIFIER TOK_LEFT_PAREN expr_seq TOK_RIGHT_PAREN  { }
     ;

func_def:  TOK_DEF TOK_IDENTIFIER TOK_LEFT_PAREN parameters TOK_RIGHT_PAREN TOK_COLON stmt_seq TOK_ENDEF { }
	 ;

parameters:  nonempty_parameters { }
	   | { }
	   ;

nonempty_parameters:  TOK_IDENTIFIER { }
		    | TOK_IDENTIFIER TOK_COMMA nonempty_parameters { }
		    ;

%%

void yyerror(YYLTYPE *locp, const char *msg)
{
	
}
