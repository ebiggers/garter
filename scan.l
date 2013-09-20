 /* Definitions section  */

%{
	#include "parse.h"
	#define YY_USER_ACTION yylloc->first_line = yylineno;
%}

%option bison-bridge
%option bison-locations
%option nodefault
%option noyywrap
%option prefix="garter_"
%option reentrant
%option warn
%option yylineno

%%
 /* Rules section  */

and		return TOK_AND;
def		return TOK_DEF;
elif		return TOK_ELIF;
else		return TOK_ELSE;
endef		return TOK_ENDEF;
endfor		return TOK_ENDFOR;
endif		return TOK_ENDIF;
endwhile	return TOK_ENDWHILE;
False		return TOK_FALSE;
for		return TOK_FOR;
if		return TOK_IF;
in		return TOK_IN;
not		return TOK_NOT;
or		return TOK_OR;
pass		return TOK_PASS;
print		return TOK_PRINT;
return		return TOK_RETURN;
True		return TOK_TRUE;
while		return TOK_WHILE;
[0-9]+		return TOK_INTEGER;
[_a-zA-Z][_a-zA-Z0-9]*		return TOK_IDENTIFIER;
\(		return TOK_LEFT_PAREN;
\)		return TOK_RIGHT_PAREN;
:		return TOK_COLON;
;		return TOK_SEMICOLON;
,		return TOK_COMMA;
\[		return TOK_LEFT_SQUARE_BRACKET;
\]		return TOK_RIGHT_SQUARE_BRACKET;
=		return TOK_ASSIGNMENT;
\+		return TOK_PLUS;
-		return TOK_MINUS;
\*		return TOK_ASTERISK;
\/		return TOK_FORWARD_SLASH;
%		return TOK_PERCENT;
\*\*		return TOK_DOUBLE_ASTERISK;
\<		return TOK_LT;
\>		return TOK_GT;
\<=		return TOK_LE;
\>=		return TOK_GE;
==		return TOK_EQ;
!=		return TOK_NEQ;
#.*$		{ /* Comment  */ }
[[:space:]]	{ /* Whitespace  */  }
.		{ /* TODO:  Bad input  */ }

%%
 /* User code section  */
