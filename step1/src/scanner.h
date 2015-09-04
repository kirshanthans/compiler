#ifndef _SCANNER
#define _SCANNER

extern FILE*  yyin;
extern char*  yytext;
extern int    yylex(void);

enum TOKEN 
{
t_EOF           = 0,
t_KEYWORD       = 1,
t_OPERATOR      = 2,
t_COMMENT       = 3,
t_STRINGLITERAL = 4,
t_IDENTIFIER    = 5,
t_FLOATLITERAL  = 6,
t_INTLITERAL    = 7,
t_WHITESPACE    = 8,
t_ILLEGAL       = 9
};

#endif
