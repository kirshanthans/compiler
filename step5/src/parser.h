#ifndef _PARSER
#define _PARSER

#include <stdio.h>
#include <stdlib.h>

extern FILE*  yyin;
extern char*  yytext;      
extern int    yylex(void); 
extern void   yyerror(char* s);
extern int    yyparse(void);
#endif
