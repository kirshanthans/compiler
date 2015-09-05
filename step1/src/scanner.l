%{
#include"../src/scanner.h"
%}

KEYWORD			"PROGRAM"|"BEGIN"|"END"|"FUNCTION"|"READ"|"WRITE"|"IF"|"ELSE"|"FI"|"FOR"|"ROF"|"CONTINUE"|"BREAK"|"RETURN"|"INT"|"VOID"|"STRING"|"FLOAT"
OPERATOR 		":="|"+"|"-"|"*"|"/"|"="|"!="|"<"|">"|"("|")"|";"|","|"<="|">="
DIGIT			[0-9]
LETTER			[A-Za-z]
COMMENT			--.*\n
STRINGLITERAL	\"[^\"]*\"
FLOATLITERAL	{DIGIT}*"."{DIGIT}+
INTLITERAL		{DIGIT}+
IDENTIFIER 		{LETTER}({LETTER}|{DIGIT})*
WHITESPACE      [ \n\t\r\n]+

%%

{KEYWORD}                            { return TOKEN_KEYWORD; }
{OPERATOR}                           { return TOKEN_OPERATOR; }
{COMMENT}                            { ; }
{STRINGLITERAL}                      { return TOKEN_STRINGLITERAL;}
{IDENTIFIER}                         { return TOKEN_IDENTIFIER;}
{FLOATLITERAL}                       { return TOKEN_FLOATLITERAL;}
{INTLITERAL}                         { return TOKEN_INTLITERAL;}
{WHITESPACE}                         { ; }
. 									 { return TOKEN_ILLEGAL;}
%%
int yywrap()
{
		return 1; 
}

