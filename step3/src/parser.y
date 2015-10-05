%{
#include "parser.h"
#include "symboltable.h"
#include <string.h>
Scope* currentScope;
vector<string> idList; 
%}

%start program

%union {char* sval; int ival; float fval;}

%token TOKEN_KEY_PROGRAM  
%token TOKEN_KEY_BEGIN    
%token TOKEN_KEY_END      
%token TOKEN_KEY_FUNCTION 
%token TOKEN_KEY_READ     
%token TOKEN_KEY_WRITE    
%token TOKEN_KEY_IF       
%token TOKEN_KEY_ELSE     
%token TOKEN_KEY_FI       
%token TOKEN_KEY_FOR      
%token TOKEN_KEY_ROF      
%token TOKEN_KEY_CONTINUE 
%token TOKEN_KEY_BREAK    
%token TOKEN_KEY_RETURN   
%token TOKEN_KEY_INT      
%token TOKEN_KEY_VOID     
%token TOKEN_KEY_STRING   
%token TOKEN_KEY_FLOAT    

%token TOKEN_OP_ASSIGN  
%token TOKEN_OP_ADD     
%token TOKEN_OP_SUB     
%token TOKEN_OP_MUL     
%token TOKEN_OP_DIV     
%token TOKEN_OP_EQ      
%token TOKEN_OP_NEQ     
%token TOKEN_OP_LE      
%token TOKEN_OP_GR      
%token TOKEN_OP_LEQ     
%token TOKEN_OP_GEQ     
%token TOKEN_OP_LPAREN  
%token TOKEN_OP_RPAREN  
%token TOKEN_OP_SEMICOL 
%token TOKEN_OP_COMMA   

%token TOKEN_STRINGLITERAL 
%token TOKEN_IDENTIFIER    
%token TOKEN_FLOATLITERAL  
%token TOKEN_INTLITERAL    

%token TOKEN_ILLEGAL

%type <sval> id
%type <sval> str
%type <ival> var_type

%%

/* Program */
program  : TOKEN_KEY_PROGRAM id{currentScope = createGlobalScope();} TOKEN_KEY_BEGIN pgm_body TOKEN_KEY_END{currentScope->printShadowVar();currentScope->printSymbolTable();}
		 ;
id       : TOKEN_IDENTIFIER{$$ = strdup(yytext);}
		 ;
pgm_body : decl func_declarations
		 ;
decl     : string_decl decl
	     | var_decl decl
         | empty
	     ;

/* Global String Declaration */
string_decl : TOKEN_KEY_STRING id TOKEN_OP_ASSIGN str TOKEN_OP_SEMICOL{bool status = currentScope->addSymbolEntry($2,$4); if(!status) return 1;}
			;
str         : TOKEN_STRINGLITERAL{$$ = strdup(yytext);}
			;

/* Variable Declaration */
var_decl : var_type id_list TOKEN_OP_SEMICOL  
		 {
		 	for(int i = idList.size()-1; i >= 0; i--){
				bool status = currentScope->addSymbolEntry(idList[i], static_cast<SymbolEntryType>($<ival>1));
				if(!status) return 1;
		 	}
		 }
		 ;
var_type  : TOKEN_KEY_FLOAT{$$ = FLOAT;}
		  | TOKEN_KEY_INT{$$ = INT;}
		  ;
any_type  : var_type 
		  | TOKEN_KEY_VOID
		  ;
id_list   : id{idList.clear();} id_tail{idList.push_back($1);}
		  ;
id_tail   : TOKEN_OP_COMMA id id_tail{idList.push_back($2);}
		  | empty
		  ;

/* Function Paramater List */
param_decl_list : param_decl param_decl_tail 
				| empty
				;
param_decl      : var_type id
				{
					bool status = currentScope->addSymbolEntry($2, static_cast<SymbolEntryType>($<ival>1));
					if(!status) return 1;
				}
				;
param_decl_tail : TOKEN_OP_COMMA param_decl param_decl_tail 
				| empty
				;

/* Function Declarations */
func_declarations : func_decl func_declarations 
				  | empty
				  ;
func_decl         : TOKEN_KEY_FUNCTION any_type id{currentScope = currentScope->createChildScope($3);} TOKEN_OP_LPAREN param_decl_list TOKEN_OP_RPAREN TOKEN_KEY_BEGIN func_body TOKEN_KEY_END{currentScope = currentScope->getParentScope();}
				  ;
func_body         : decl stmt_list 
				  ;

/* Statement List */
stmt_list         : stmt stmt_list 
				  | empty
				  ;
stmt              : base_stmt 
				  | if_stmt 
				  | for_stmt
				  ;
base_stmt         : assign_stmt 
				  | read_stmt 
				  | write_stmt 
                  | return_stmt
				  ;

/* Basic Statements */
assign_stmt       : assign_expr TOKEN_OP_SEMICOL
				  ;
assign_expr       : id TOKEN_OP_ASSIGN expr
				  ;
read_stmt         : TOKEN_KEY_READ TOKEN_OP_LPAREN id_list TOKEN_OP_RPAREN TOKEN_OP_SEMICOL
				  ;
write_stmt        : TOKEN_KEY_WRITE TOKEN_OP_LPAREN id_list TOKEN_OP_RPAREN TOKEN_OP_SEMICOL
				  ;
return_stmt       : TOKEN_KEY_RETURN expr TOKEN_OP_SEMICOL
				  ;

/* Expressions */
expr              : expr_prefix factor
				  ;
expr_prefix       : expr_prefix factor addop 
				  | empty
				  ;
factor            : factor_prefix postfix_expr
				  ;
factor_prefix     : factor_prefix postfix_expr mulop 
				  | empty
				  ;
postfix_expr      : primary 
				  | call_expr
				  ;
call_expr         : id TOKEN_OP_LPAREN expr_list TOKEN_OP_RPAREN
				  ;
expr_list         : expr expr_list_tail 
				  | empty
				  ;
expr_list_tail    : TOKEN_OP_COMMA expr expr_list_tail 
				  | empty
				  ;
primary           : TOKEN_OP_LPAREN expr TOKEN_OP_RPAREN 
				  | id 
                  | TOKEN_INTLITERAL 
                  | TOKEN_FLOATLITERAL
				  ;
addop             : TOKEN_OP_ADD 
				  | TOKEN_OP_SUB
				  ;
mulop             : TOKEN_OP_MUL 
				  | TOKEN_OP_DIV 
				  ;

/* Complex Statements and Condition */ 
if_stmt     : TOKEN_KEY_IF TOKEN_OP_LPAREN cond TOKEN_OP_RPAREN{currentScope = currentScope->createChildScope();} decl stmt_list{currentScope = currentScope->getParentScope();} else_part TOKEN_KEY_FI
			;
else_part   : TOKEN_KEY_ELSE{currentScope = currentScope->createChildScope();} decl stmt_list {currentScope = currentScope->getParentScope();}
			| empty
			;
cond        : expr compop expr;
compop      : TOKEN_OP_GR 
		  	| TOKEN_OP_LE 
            | TOKEN_OP_EQ 
            | TOKEN_OP_NEQ 
            | TOKEN_OP_LEQ 
            | TOKEN_OP_GEQ
            ;
init_stmt   : assign_expr 
		  	| empty
			;
incr_stmt   : assign_expr 
	        | empty
			;

/* ECE 573 students use this version of for_stmt */
for_stmt : TOKEN_KEY_FOR TOKEN_OP_LPAREN init_stmt TOKEN_OP_SEMICOL cond TOKEN_OP_SEMICOL incr_stmt TOKEN_OP_RPAREN{currentScope = currentScope->createChildScope();} decl aug_stmt_list TOKEN_KEY_ROF{currentScope = currentScope->getParentScope();}
		 ;

/* Continue and Break statements*/
aug_stmt_list : aug_stmt aug_stmt_list 
		  	  | empty
			  ;
aug_stmt      : base_stmt 
		  	  | aug_if_stmt 
              | for_stmt 
              | TOKEN_KEY_CONTINUE TOKEN_OP_SEMICOL 
              | TOKEN_KEY_BREAK TOKEN_OP_SEMICOL
			  ;

/* Augmented if statements*/ 
aug_if_stmt   : TOKEN_KEY_IF TOKEN_OP_LPAREN cond TOKEN_OP_RPAREN {currentScope = currentScope->createChildScope();} decl aug_stmt_list{currentScope = currentScope->getParentScope();} aug_else_part TOKEN_KEY_FI
		      ;
aug_else_part : TOKEN_KEY_ELSE{currentScope = currentScope->createChildScope();} decl aug_stmt_list {currentScope = currentScope->getParentScope();} 
		      | empty
			  ;

/* Empty  Token*/
empty : 
	  ;
%%
void yyerror(char* s)
{
	//fprintf(stderr, "error: %s\n", s);
}
