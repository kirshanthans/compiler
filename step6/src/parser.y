%{
#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "symboltable.h"
#include "astnode.h"
#include "basicblocks.h"
#include "optimizer.h"
#include "codegen.h"

Scope* currentScope;
vector<string> idList; 
vector<ASTNode*> expList; 
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
%type <ival> var_type mulop addop compop any_type
%type <astval> stmt_list stmt base_stmt for_stmt assign_stmt assign_expr expr factor postfix_expr primary factor_prefix expr_prefix write_stmt read_stmt if_stmt cond else_part aug_stmt_list aug_stmt aug_else_part aug_if_stmt return_stmt init_stmt incr_stmt func_body call_expr

%%

/* Program */
program  : TOKEN_KEY_PROGRAM id{currentScope = Scope::createGlobalScope();} TOKEN_KEY_BEGIN pgm_body TOKEN_KEY_END
		 {
		 	vector<IRNode> code;
			vector<AssemblyNode> assembly;
			code = CodeGen::genAllCode();
			assembly = genAssembly(code);
		    cout << convertCodetoString(code,"*****IR Start*****","*****IR End*****");
			cout << ";*****Tiny Code*****" << endl;
			cout << convertAssemblytoString(globalDecl, false);
			cout << convertAssemblytoString(assembly, false);
		 }
		 ;
id       : TOKEN_IDENTIFIER
		 {
			$<sval>$ = strdup(yytext);
		 }
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
				bool status = currentScope->addSymbolEntry($<sval>2,$<sval>4,false);
				if(!status) return 1;
			}
			;
str         : TOKEN_STRINGLITERAL{$<sval>$ = strdup(yytext);}
			;

/* Variable Declaration */
var_decl  : var_type id_list TOKEN_OP_SEMICOL  
		  {
			for(int i = idList.size()-1; i >= 0; i--){
					bool status = currentScope->addSymbolEntry(idList[i], static_cast<SymbolEntryType>($<ival>1), false);
		    		if(!status) return 1;
		  	}
		  }
		  ;
var_type  : TOKEN_KEY_FLOAT{$<ival>$ = SymbolEntryType::FLOAT;}
		  | TOKEN_KEY_INT{$<ival>$ = SymbolEntryType::INT;}
		  ;
any_type  : var_type {$<ival>$=$<ival>1;}
          | TOKEN_KEY_VOID {$<ival>$=SymbolEntryType::VOID;}
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
					bool status = currentScope->addSymbolEntry($<sval>2, static_cast<SymbolEntryType>($<ival>1), true);
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
func_decl         : TOKEN_KEY_FUNCTION any_type id{currentScope = currentScope->createChildScope($<sval>3, static_cast<SymbolEntryType>($<ival>2));} TOKEN_OP_LPAREN  param_decl_list TOKEN_OP_RPAREN TOKEN_KEY_BEGIN func_body TOKEN_KEY_END
				  {
					CodeGen::genarateCodeForFunction($9,$<sval>3); 
					currentScope = currentScope->getParentScope();
				  }
				  ;
func_body         : decl stmt_list{$$=$2;}
				  ;

/* Statement List */
stmt_list         : stmt stmt_list{$$ = new StatementList($1,$2);}
				  | empty{$$ = new StatementList(NULL,NULL);}
				  ;
stmt              : base_stmt{$$ = $1;}
		  		  | if_stmt{$$ = $1;}
		  		  | for_stmt{$$ = $1;}
				  ;
base_stmt         : assign_stmt{$$ = $1;} 
		  		  | read_stmt{$$ = $1;} 
		  		  | write_stmt{$$ = $1;} 
                  | return_stmt{$$ = $1;}
				  ;

/* Basic Statements */
assign_stmt       : assign_expr TOKEN_OP_SEMICOL{$$ = $1;} 
				  ;
assign_expr       : id TOKEN_OP_ASSIGN expr{$$ = new AssignStatement(new Identifier($<sval>1,currentScope), $3);}
				  ;
read_stmt         : TOKEN_KEY_READ{idList.clear();} TOKEN_OP_LPAREN id_list TOKEN_OP_RPAREN TOKEN_OP_SEMICOL
				  {
						vector<ASTNode*> idASTList;
						for(int i=idList.size()-1; i>=0; i--){
								idASTList.push_back(new Identifier(idList[i],currentScope));
						}
						$$ = new IOCommand(true, idASTList);
				  }
				  ;
write_stmt        : TOKEN_KEY_WRITE{idList.clear();}TOKEN_OP_LPAREN id_list TOKEN_OP_RPAREN TOKEN_OP_SEMICOL
				  {
						vector<ASTNode*> idASTList;
						for(int i=idList.size()-1; i>=0; i--){
								idASTList.push_back(new Identifier(idList[i],currentScope));
						}
						$$ = new IOCommand(false, idASTList);
				  }
				  ;
return_stmt       : TOKEN_KEY_RETURN expr TOKEN_OP_SEMICOL{$$ = new Return($2, currentScope);}
				  ;

/* Expressions */
expr              : expr_prefix factor
				  {
				  		ASTNode* left;
				  		if($1 != NULL){
				  			left = $1;
				  			left->right = $2;
				  		}
				  		else{
				  			left = $2;
				  		}
				  		$$ = left;
				  }
				  ;
expr_prefix       : expr_prefix factor addop 
				  {
				  		ASTNode* left;
				  		if($1 != NULL){
				  			left=$1;
				  			left->right = $2;
				  		}
				  		else{
				  			left=$2;
				  		}
				  		$$ = new Binop(left,NULL, static_cast<BinopType>($<ival>3));
				  } 
				  | empty{$$ = NULL;}
 				  ;
factor            : factor_prefix postfix_expr
				  {
						ASTNode* left;
						if($1 != NULL){
							left = $1;
							left->right = $2;
						}
						else{
							left = $2;
						}
						$$ = left;
				  }
				  ;
factor_prefix     : factor_prefix postfix_expr mulop
				  {
				  		ASTNode* left;
				  		if($1 != NULL){
				  			left=$1;
				  			left->right = $2;
				  		}
				  		else{
				  			left=$2;
				  		}
				  		$$ = new Binop(left,NULL, static_cast<BinopType>($<ival>3));
				  } 
		  		  | empty {$$ = NULL;}
				  ;
postfix_expr      : primary {$$ = $1;}
		   		  | call_expr{$$ = $1;}
				  ;
call_expr         : id{expList.clear();} TOKEN_OP_LPAREN  expr_list TOKEN_OP_RPAREN{$$ = new FuncCall($1, expList);}
				  ;
expr_list         : expr{expList.clear();} expr_list_tail{expList.insert(expList.begin(), $1);}
				  | empty
				  ;
expr_list_tail    : TOKEN_OP_COMMA expr expr_list_tail{expList.insert(expList.begin(), $2);} 
				  | empty
				  ;
primary           : TOKEN_OP_LPAREN expr TOKEN_OP_RPAREN{$$ = $2;}
		  		  | id{$$ = new Identifier($<sval>1, currentScope);}
                  | TOKEN_INTLITERAL  { $$ = new Constant(strdup(yytext),SymbolEntryType::INT); }
                  | TOKEN_FLOATLITERAL{ $$ = new Constant(strdup(yytext),SymbolEntryType::FLOAT);}
				  ;
addop             : TOKEN_OP_ADD {$<ival>$ = BinopType::ADD;}
		  		  | TOKEN_OP_SUB {$<ival>$ = BinopType::SUB;}
				  ;
mulop             : TOKEN_OP_MUL {$<ival>$ = BinopType::MUL;}
				  | TOKEN_OP_DIV {$<ival>$ = BinopType::DIV;}
				  ;

/* Complex Statements and Condition */ 
if_stmt     : TOKEN_KEY_IF TOKEN_OP_LPAREN cond TOKEN_OP_RPAREN{currentScope = currentScope->createChildScope();} decl stmt_list {currentScope = currentScope->getParentScope();} else_part TOKEN_KEY_FI
			{
				$$ = new IfStatment($3,$7,$9); 
			}
			;
else_part   : TOKEN_KEY_ELSE{currentScope = currentScope->createChildScope();} decl stmt_list {currentScope = currentScope->getParentScope();}
			{
				$$=$4;
			}
			| empty{$$ = new StatementList(NULL,NULL);}
			;
cond        : expr compop expr{$$ = new ConditionExpression($1,$3,static_cast<CompopType>($<ival>2));}
			;
compop      : TOKEN_OP_GR  {$<ival>$ = CompopType::COMP_GR;}
	    	| TOKEN_OP_LE  {$<ival>$ = CompopType::COMP_LE;}
            | TOKEN_OP_EQ  {$<ival>$ = CompopType::COMP_EQ;}
            | TOKEN_OP_NEQ {$<ival>$ = CompopType::COMP_NEQ;}
            | TOKEN_OP_LEQ {$<ival>$ = CompopType::COMP_LEQ;}
            | TOKEN_OP_GEQ {$<ival>$ = CompopType::COMP_GEQ;}
            ;
init_stmt   : assign_expr {$$=$1;}
	    	| empty {$$ = new StatementList(NULL,NULL);}
			;
incr_stmt   : assign_expr {$$=$1;}
	    	| empty {$$ = new StatementList(NULL,NULL);}
			;

/* ECE 573 students use this version of for_stmt */
for_stmt : TOKEN_KEY_FOR TOKEN_OP_LPAREN init_stmt TOKEN_OP_SEMICOL cond TOKEN_OP_SEMICOL incr_stmt TOKEN_OP_RPAREN{currentScope = currentScope->createChildScope();} decl aug_stmt_list TOKEN_KEY_ROF{currentScope = currentScope->getParentScope();}
         {
			$$ = new ForStatment($3,$5,$7,$11); 
         }	 
         ;
/* Continue and Break statements*/
aug_stmt_list : aug_stmt aug_stmt_list  {$$ = new StatementList($1,$2);}
	      	  | empty{$$ = new StatementList(NULL,NULL);}
			  ;
aug_stmt      : base_stmt   {$$ = $1;} 
	      	  | aug_if_stmt {$$ = $1;}
              | for_stmt    {$$ = $1;}
              | TOKEN_KEY_CONTINUE TOKEN_OP_SEMICOL {$$ = new LoopControl(false);}
              | TOKEN_KEY_BREAK TOKEN_OP_SEMICOL    {$$ = new LoopControl(true);}
			  ;

/* Augmented if statements*/ 
aug_if_stmt   : TOKEN_KEY_IF TOKEN_OP_LPAREN cond TOKEN_OP_RPAREN {currentScope = currentScope->createChildScope();} decl aug_stmt_list{currentScope = currentScope->getParentScope();} aug_else_part TOKEN_KEY_FI
			  {
				$$ = new IfStatment($3,$7,$9); 
			  }		      
			  ;
aug_else_part : TOKEN_KEY_ELSE{currentScope = currentScope->createChildScope();} decl aug_stmt_list {currentScope = currentScope->getParentScope();} 
			  {
				$$ = $4;
			  }	 
			  | empty{$$ = new StatementList(NULL,NULL);}
			  ;

/* Empty  Token*/
empty : 
	  ;
%%
void yyerror(char* s)
{
}
