%{
#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "symboltable.h"
#include "astnode.h"
#include "optimizer.h"
Scope* currentScope;
vector<string> idList; 
vector<AssemblyNode> globalDecl;
using namespace std;
%}

%start program

%union {char* sval; int ival; float fval; ASTNode* astval;}

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

%type <sval> id str
%type <ival> var_type mulop addop
%type <astval> stmt_list stmt base_stmt if_stmt for_stmt assign_stmt assign_expr expr factor postfix_expr primary factor_prefix expr_prefix write_stmt read_stmt
%%

/* Program */
program  : TOKEN_KEY_PROGRAM id{currentScope = createGlobalScope();} TOKEN_KEY_BEGIN pgm_body TOKEN_KEY_END
		 ;
id       : TOKEN_IDENTIFIER{$<sval>$ = strdup(yytext);}
		 ;
pgm_body : decl{globalDecl = declAssembly(currentScope);} func_declarations
		 ;
decl     : string_decl decl
	     | var_decl decl
         | empty
	     ;

/* Global String Declaration */
string_decl : TOKEN_KEY_STRING id TOKEN_OP_ASSIGN str TOKEN_OP_SEMICOL
			{
				bool status = currentScope->addSymbolEntry($<sval>2,$<sval>4);
				if(!status) return 1;
			}
			;
str         : TOKEN_STRINGLITERAL{$<sval>$ = strdup(yytext);}
			;

/* Variable Declaration */
var_decl  : var_type id_list TOKEN_OP_SEMICOL  
		  {
				for(int i = idList.size()-1; i >= 0; i--){
						bool status = currentScope->addSymbolEntry(idList[i], static_cast<SymbolEntryType>($<ival>1));
		     			if(!status) return 1;
		  		}
		  }
		  ;
var_type  : TOKEN_KEY_FLOAT{$<ival>$ = FLOAT;}
		  | TOKEN_KEY_INT{$<ival>$ = INT;}
		  ;
any_type  : var_type 
		  | TOKEN_KEY_VOID
		  ;
id_list   : id{idList.clear();} id_tail{idList.push_back($<sval>1);}
		  ;
id_tail   : TOKEN_OP_COMMA id id_tail{idList.push_back($<sval>2);}
		  | empty
		  ;

/* Function Paramater List */
param_decl_list : param_decl param_decl_tail 
				| empty
				;
param_decl      : var_type id
				{
					bool status = currentScope->addSymbolEntry($<sval>2, static_cast<SymbolEntryType>($<ival>1));
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
func_decl         : TOKEN_KEY_FUNCTION any_type id{currentScope = currentScope->createChildScope($<sval>3);} TOKEN_OP_LPAREN param_decl_list TOKEN_OP_RPAREN TOKEN_KEY_BEGIN func_body TOKEN_KEY_END{currentScope = currentScope->getParentScope();}
				  ;
func_body         : decl stmt_list
				  {
						vector<IRNode> code;						
						if($<astval>2){						
						code = $<astval>2->genCode();
						Optimizer::removeRedundantIR(code);	
						Optimizer::removeRedundantExpression(code);
						Optimizer::optimizeInstructionSelection(code);
						Optimizer::removeRedundantExpression(code);
						Optimizer::removeRedundantIR(code);	
						cout << convertCodetoString(code);
						}
						vector<AssemblyNode> assembly = genAssembly(code);
						cout << convertAssemblytoString(globalDecl);
						cout << convertAssemblytoString(assembly);
				  }
				  ;

/* Statement List */
stmt_list         : stmt stmt_list{$<astval>$ = new StatementList($1,$2);}
				  | empty{$<astval>$ = NULL;}
				  ;
stmt              : base_stmt{$<astval>$ = $1;}
				  | if_stmt{$<astval>$ = NULL;}
				  | for_stmt{$<astval>$ = NULL;}
				  ;
base_stmt         : assign_stmt{$<astval>$ = $<astval>1;} 
		  		  | read_stmt{$<astval>$ = $<astval>1;} 
		  		  | write_stmt{$<astval>$ = $<astval>1;} 
                  | return_stmt{$<astval>$ = NULL;}
				  ;

/* Basic Statements */
assign_stmt       : assign_expr TOKEN_OP_SEMICOL{$<astval>$ = $<astval>1;} 
				  ;
assign_expr       : id TOKEN_OP_ASSIGN expr{$<astval>$ = new AssignStatement(new Identifier($<sval>1,currentScope), $<astval>3);}
				  ;
read_stmt         : TOKEN_KEY_READ{idList.clear();} TOKEN_OP_LPAREN id_list TOKEN_OP_RPAREN TOKEN_OP_SEMICOL
				  {
						vector<ASTNode*> idASTList;
						for(int i=idList.size()-1; i>=0; i--){
								idASTList.push_back(new Identifier(idList[i],currentScope));
						}
						$<astval>$=new IOCommand(true, idASTList);
				  }
				  ;
write_stmt        : TOKEN_KEY_WRITE {idList.clear();}TOKEN_OP_LPAREN id_list TOKEN_OP_RPAREN TOKEN_OP_SEMICOL
				  {
						vector<ASTNode*> idASTList;
						for(int i=idList.size()-1; i>=0; i--){
								idASTList.push_back(new Identifier(idList[i],currentScope));
						}
						$<astval>$=new IOCommand(false, idASTList);
				  }
				  ;
return_stmt       : TOKEN_KEY_RETURN expr TOKEN_OP_SEMICOL{$<astval>$ = NULL;}
				  ;

/* Expressions */
expr              : expr_prefix factor
				  {
				  		ASTNode* left;
				  		if($<astval>1 != NULL){
				  			left = $<astval>1;
				  			left->right = $<astval>2;
				  		}
				  		else{
				  			left = $<astval>2;
				  		}
				  		$<astval>$ = left;
				  }
				  ;
expr_prefix       : expr_prefix factor addop 
				  {
				  		ASTNode* left;
				  		if($<astval>1 != NULL){
				  			left=$<astval>1;
				  			left->right = $<astval>2;
				  		}
				  		else{
				  			left=$<astval>2;
				  		}
				  		$<astval>$ = new Binop(left,NULL, static_cast<BinopType>($<ival>3));
				  } 
				  | empty{$<astval>$ = NULL;}
 				  ;
factor            : factor_prefix postfix_expr
				  {
						ASTNode* left;
						if($<astval>1 != NULL){
							left = $<astval>1;
							left->right = $<astval>2;
						}
						else{
							left = $<astval>2;
						}
						$<astval>$ = left;
				  }
				  ;
factor_prefix     : factor_prefix postfix_expr mulop
				  {
				  		ASTNode* left;
				  		if($<astval>1 != NULL){
				  			left=$<astval>1;
				  			left->right = $<astval>2;
				  		}
				  		else{
				  			left=$<astval>2;
				  		}
				  		$<astval>$ = new Binop(left,NULL, static_cast<BinopType>($<ival>3));
				  } 
		  		  | empty {$<astval>$ = NULL;}
				  ;
postfix_expr      : primary {$<astval>$ = $<astval>1;}
		   		  | call_expr{$<astval>$ = NULL;}
				  ;
call_expr         : id TOKEN_OP_LPAREN expr_list TOKEN_OP_RPAREN
				  ;
expr_list         : expr expr_list_tail 
				  | empty
				  ;
expr_list_tail    : TOKEN_OP_COMMA expr expr_list_tail 
				  | empty
				  ;
primary           : TOKEN_OP_LPAREN expr TOKEN_OP_RPAREN{$<astval>$ = $<astval>2;}
		  		  | id{$<astval>$ = new Identifier($<sval>1, currentScope);}
                  | TOKEN_INTLITERAL  { $<astval>$ = new Constant(strdup(yytext),SymbolEntryType::INT); }
                  | TOKEN_FLOATLITERAL{ $<astval>$ = new Constant(strdup(yytext),SymbolEntryType::FLOAT);}
;
				  ;
addop             : TOKEN_OP_ADD {$<ival>$ = BinopType::ADD;}
		  		  | TOKEN_OP_SUB {$<ival>$ = BinopType::SUB;}
				  ;
mulop             : TOKEN_OP_MUL {$<ival>$ = BinopType::MUL;}
				  | TOKEN_OP_DIV {$<ival>$ = BinopType::DIV;}
				  ;

/* Complex Statements and Condition */ 
if_stmt     : TOKEN_KEY_IF TOKEN_OP_LPAREN cond TOKEN_OP_RPAREN{currentScope = currentScope->createChildScope();} decl stmt_list{currentScope = currentScope->getParentScope();} else_part TOKEN_KEY_FI
			{$<astval>$ = NULL;}
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
