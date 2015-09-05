#ifndef _SCANNER
#define _SCANNER

extern FILE*  yyin;
extern char*  yytext;
extern int    yylex(void);

#define TOKEN_EOF            0
#define TOKEN_KEYWORD        1
#define TOKEN_OPERATOR       2
#define TOKEN_COMMENT        3
#define TOKEN_STRINGLITERAL  4
#define TOKEN_IDENTIFIER     5
#define TOKEN_FLOATLITERAL   6
#define TOKEN_INTLITERAL     7
#define TOKEN_WHITESPACE     8
#define TOKEN_ILLEGAL        9

#endif
