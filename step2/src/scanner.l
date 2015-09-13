%{
#include "scanner.h"
%}

KEY_PROGRAM  "PROGRAM"
KEY_BEGIN    "BEGIN"
KEY_END      "END"
KEY_FUNCTION "FUNCTION"
KEY_READ     "READ"
KEY_WRITE    "WRITE"
KEY_IF       "IF"
KEY_ELSE     "ELSE"
KEY_FI       "FI"
KEY_FOR      "FOR"
KEY_ROF      "ROF"
KEY_CONTINUE "CONTINUE"
KEY_BREAK    "BREAK"
KEY_RETURN   "RETURN"
KEY_INT      "INT"
KEY_VOID     "VOID"
KEY_STRING   "STRING"
KEY_FLOAT    "FLOAT"

OP_ASSIGN  ":="
OP_ADD     "+"
OP_SUB     "-"
OP_MUL     "*"
OP_DIV     "/"
OP_EQ      "="
OP_NEQ     "!="
OP_LE      "<"
OP_GR      ">"
OP_LEQ     "<="
OP_GEQ     ">="
OP_LPAREN  "("
OP_RPAREN  ")"
OP_SEMICOL ";"
OP_COMMA   ","

DIGIT			[0-9]
LETTER			[A-Za-z]
COMMENT			--.*\n
STRINGLITERAL	\"[^\"]*\"
FLOATLITERAL	{DIGIT}*"."{DIGIT}+
INTLITERAL		{DIGIT}+
IDENTIFIER 		{LETTER}({LETTER}|{DIGIT})*
WHITESPACE      [ \n\t\r\n]+

%%
{KEY_PROGRAM}  { return TOKEN_KEY_PROGRAM  ; }
{KEY_BEGIN}    { return TOKEN_KEY_BEGIN    ; }
{KEY_END}      { return TOKEN_KEY_END      ; }
{KEY_FUNCTION} { return TOKEN_KEY_FUNCTION ; }
{KEY_READ}     { return TOKEN_KEY_READ     ; }
{KEY_WRITE}    { return TOKEN_KEY_WRITE    ; }
{KEY_IF}       { return TOKEN_KEY_IF       ; }
{KEY_ELSE}     { return TOKEN_KEY_ELSE     ; }
{KEY_FI}       { return TOKEN_KEY_FI       ; }
{KEY_FOR}      { return TOKEN_KEY_FOR      ; }
{KEY_ROF}      { return TOKEN_KEY_ROF      ; }
{KEY_CONTINUE} { return TOKEN_KEY_CONTINUE ; }
{KEY_BREAK}    { return TOKEN_KEY_BREAK    ; }
{KEY_RETURN}   { return TOKEN_KEY_RETURN   ; }
{KEY_INT}      { return TOKEN_KEY_INT      ; }
{KEY_VOID}     { return TOKEN_KEY_VOID     ; }
{KEY_STRING}   { return TOKEN_KEY_STRING   ; }
{KEY_FLOAT}    { return TOKEN_KEY_FLOAT    ; }

{OP_ASSIGN}  { return TOKEN_OP_ASSIGN  ; }
{OP_ADD}     { return TOKEN_OP_ADD     ; }
{OP_SUB}     { return TOKEN_OP_SUB     ; }
{OP_MUL}     { return TOKEN_OP_MUL     ; }
{OP_DIV}     { return TOKEN_OP_DIV     ; }
{OP_EQ}      { return TOKEN_OP_EQ      ; }
{OP_NEQ}     { return TOKEN_OP_NEQ     ; }
{OP_LE}      { return TOKEN_OP_LE      ; }
{OP_GR}      { return TOKEN_OP_GR      ; }
{OP_LEQ}     { return TOKEN_OP_LEQ     ; }
{OP_GEQ}     { return TOKEN_OP_GEQ     ; }
{OP_LPAREN}  { return TOKEN_OP_LPAREN  ; }
{OP_RPAREN}  { return TOKEN_OP_RPAREN  ; }
{OP_SEMICOL} { return TOKEN_OP_SEMICOL ; }
{OP_COMMA}   { return TOKEN_OP_COMMA   ; }

{STRINGLITERAL}                      { return TOKEN_STRINGLITERAL ; }
{IDENTIFIER}                         { return TOKEN_IDENTIFIER    ; }
{FLOATLITERAL}                       { return TOKEN_FLOATLITERAL  ; }
{INTLITERAL}                         { return TOKEN_INTLITERAL    ; }
{COMMENT}                            { ; }
{WHITESPACE}                         { ; }

. 									 { return TOKEN_ILLEGAL;}
%%
int yywrap()
{
		return 1; 
}

