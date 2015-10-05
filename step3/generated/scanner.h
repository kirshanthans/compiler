/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_GENERATED_SCANNER_H_INCLUDED
# define YY_YY_GENERATED_SCANNER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOKEN_KEY_PROGRAM = 258,
    TOKEN_KEY_BEGIN = 259,
    TOKEN_KEY_END = 260,
    TOKEN_KEY_FUNCTION = 261,
    TOKEN_KEY_READ = 262,
    TOKEN_KEY_WRITE = 263,
    TOKEN_KEY_IF = 264,
    TOKEN_KEY_ELSE = 265,
    TOKEN_KEY_FI = 266,
    TOKEN_KEY_FOR = 267,
    TOKEN_KEY_ROF = 268,
    TOKEN_KEY_CONTINUE = 269,
    TOKEN_KEY_BREAK = 270,
    TOKEN_KEY_RETURN = 271,
    TOKEN_KEY_INT = 272,
    TOKEN_KEY_VOID = 273,
    TOKEN_KEY_STRING = 274,
    TOKEN_KEY_FLOAT = 275,
    TOKEN_OP_ASSIGN = 276,
    TOKEN_OP_ADD = 277,
    TOKEN_OP_SUB = 278,
    TOKEN_OP_MUL = 279,
    TOKEN_OP_DIV = 280,
    TOKEN_OP_EQ = 281,
    TOKEN_OP_NEQ = 282,
    TOKEN_OP_LE = 283,
    TOKEN_OP_GR = 284,
    TOKEN_OP_LEQ = 285,
    TOKEN_OP_GEQ = 286,
    TOKEN_OP_LPAREN = 287,
    TOKEN_OP_RPAREN = 288,
    TOKEN_OP_SEMICOL = 289,
    TOKEN_OP_COMMA = 290,
    TOKEN_STRINGLITERAL = 291,
    TOKEN_IDENTIFIER = 292,
    TOKEN_FLOATLITERAL = 293,
    TOKEN_INTLITERAL = 294,
    TOKEN_ILLEGAL = 295
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 12 "src/parser.y" /* yacc.c:1909  */
char* sval; int ival; float fval;

#line 98 "generated/scanner.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_GENERATED_SCANNER_H_INCLUDED  */
