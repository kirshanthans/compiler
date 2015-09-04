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

{KEYWORD}                            { TOKEN token=t_KEYWORD;  return token; }
{OPERATOR}                           { TOKEN token=t_OPERATOR; return token; }
{COMMENT}                            { ; }
{STRINGLITERAL}                      { TOKEN token=t_STRINGLITERAL; return token;}
{IDENTIFIER}                         { TOKEN token=t_IDENTIFIER;    return token;}
{FLOATLITERAL}                       { TOKEN token=t_FLOATLITERAL;  return token;}
{INTLITERAL}                         { TOKEN token=t_INTLITERAL;    return token;}
{WHITESPACE}                         { ; }
. 									 { TOKEN token=t_ILLEGAL; return token;}
%%
int yywrap()
{
		return 1; 
}

