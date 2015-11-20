#ifndef ASTNODE_H_
#define ASTNODE_H_


#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <cstdlib>
#include"symboltable.h"

class AssemblyNode;
class IRNode;
class ASTNode;
class StatementList;
class AssignStatement;
class Identifier;
class Constant;

using namespace std;

enum Opcode      {ADDI = 0, ADDF = 1, SUBI = 2, SUBF = 3, MULTI = 4, MULTF = 5, DIVI = 6, DIVF = 7, STOREI = 8, STOREF = 9, GT = 10, GE = 11, LT = 12, LE = 13, NE = 14, EQ = 15, JUMP = 16, LABEL = 17, READI = 18, READF = 19, WRITEI = 20, WRITEF = 21, WRITES = 22 ,PUSH=23 , POP=24 , JSR=25, LINK=26,UNLINK=27 ,RET=28 , HALT=29};
enum ASTNodeType {ID = 0, BINOP = 1, STMLIST = 2, CONST = 3, ASSIGN = 4, IO = 5, IFSTMT = 6, CONDEXP = 7, FORSTMT = 8, LOOPCONTROL = 9 , FUNCALL=10, RETURN=11};
enum BinopType   {ADD = 0, SUB = 1, MUL = 2, DIV = 3};
enum CompopType  {COMP_GR = 0, COMP_LE = 1, COMP_EQ = 2, COMP_NEQ = 3, COMP_LEQ = 4, COMP_GEQ = 5};
enum AssemblyIns {ASM_ADDI = 0, ASM_ADDR = 1, ASM_SUBI = 2, ASM_SUBR = 3, ASM_MULTI = 4, ASM_MULTR = 5, ASM_DIVI = 6, ASM_DIVR = 7, ASM_MOVE = 8, ASM_READI = 9, ASM_READR = 10, ASM_WRITEI = 11, ASM_WRITER = 12, ASM_WRITES = 13, ASM_INCI = 14, ASM_DECI = 15, ASM_CMPI = 16, ASM_CMPR = 17, ASM_PUSH = 18, ASM_POP = 19, ASM_JSR = 20, ASM_RET = 21, ASM_LINK = 22, ASM_UNLINK = 23, ASM_LABEL = 24, ASM_JMP = 25, ASM_JGT = 26, ASM_JLT = 27, ASM_JGE = 28, ASM_JLE = 29, ASM_JEQ = 30, ASM_JNE = 31, ASM_VAR = 32, ASM_STR = 33, ASM_HALT = 34, ASM_END = 35};
enum OperandType {IMM = 0, TEMP = 1, MEM = 2};

string getNextTemp();
string getNextReg();
string getReg(string temp);
string generateElseLabel();

string getIROpcodeString(Opcode opcode);
string getAssemblyOpcodeString(AssemblyIns ins);
string convertCodetoString(vector<IRNode> code,string startS, string endS);
vector<AssemblyNode> declAssembly(Scope* scope);
vector<AssemblyNode> genAssembly(vector<IRNode> code);
string convertAssemblytoString(vector<AssemblyNode> code, bool isComment);

OperandType findOperandType(string op);
void flipJumpOperands(IRNode* jumpIns);

class AssemblyNode{
public:
				AssemblyIns opcode;
				string op1;
				string op2;
				AssemblyNode(AssemblyIns opcode_, string op1_, string op2_){
                    opcode = opcode_;
                    op1    = op1_;
                    op2    = op2_;
                };
				~AssemblyNode(){};
				string toString();
};

class IRNode{
public:
				Opcode opcode;
				string op1;
				string op2;
				string op3;
				bool isIntCompare; //only use for conditional jump IR
    
				IRNode(Opcode opcode_, string op1_, string op2_, string op3_){
                    opcode = opcode_;
                    op1    = op1_;
                    op2    = op2_;
                    op3    = op3_;
                };
				//todo :update gencode for cond jump IR
				IRNode(Opcode opcode_, string op1_, string op2_, string op3_, bool isIntCompare_){
                    opcode       = opcode_;
                    op1          = op1_;
                    op2          = op2_;
                    op3          = op3_;
                    isIntCompare = isIntCompare_;
                };
				~IRNode(){};
				string toIRString();
				vector<AssemblyNode> toAssembly();
				string getDef();
				bool isAUse(string def);
				bool isDef(string def);
};

class ASTNode{
public:
    
				ASTNode* left;
				ASTNode* right;
				ASTNodeType nodeType;
				SymbolEntryType valType;
				virtual vector<IRNode> genCode() = 0;
				string getTempVal(){ return tempVal; }
protected:
				string tempVal;
};

class StatementList:public ASTNode{
public:
    
				StatementList(ASTNode* left_, ASTNode* right_){
                    this->left     = left_;
                    this->right    = right_;
                    this->nodeType = ASTNodeType::STMLIST;
                }
				vector<IRNode> genCode();
    
};

class AssignStatement:public ASTNode{
public:
    
				AssignStatement(ASTNode * left_, ASTNode* right_){
                    this->left     = left_;
                    this->right    = right_;
                    this->nodeType = ASTNodeType::ASSIGN;
                    this->valType  = left_->valType;
                }
				vector<IRNode> genCode();
};

class Identifier:public ASTNode{
public:
				string name;
				Scope * scope;
				Identifier(string name_, Scope* scope_){
                    this->name     = name_;
                    this->scope    = scope_;
                    this->nodeType = ASTNodeType::ID;
                    this->valType  = scope->getIdType(name);
                }
				vector<IRNode> genCode();
};

class Constant:public ASTNode{
public:
				string valAsString;
				Constant(string valAsString_,SymbolEntryType  valType_){
                    this->valAsString = valAsString_;
                    this->valType     = valType_;
                    this->nodeType    = ASTNodeType::CONST;
                }
				vector<IRNode> genCode();
};

class IOCommand:public ASTNode{
public:
				bool isRead;
				vector<ASTNode *> idList;
				IOCommand(bool isRead_, vector<ASTNode *> idList_){
                    this->isRead   = isRead_;
                    this->nodeType = ASTNodeType::IO;
                    this->idList   = idList_;
                }
				vector<IRNode> genCode();
    
};

class Binop:public ASTNode{
public:
				BinopType operationType;
				Binop(ASTNode * left_ , ASTNode* right_, BinopType operationType_){
                    this->left          = left_;
                    this->right         = right_;
                    this->operationType = operationType_;
                    this->nodeType      = ASTNodeType::BINOP;
                    this->valType       = left_->valType;
                }
				vector<IRNode> genCode();
};

class ConditionExpression:public ASTNode{
public:
				CompopType comparisonType;
				ConditionExpression(ASTNode * left_ , ASTNode* right_, 	CompopType comparisonType_){
                    this->left           = left_;
                    this->right          = right_;
                    this->comparisonType = comparisonType_;
                    this->nodeType       = ASTNodeType::CONDEXP;
                    this->valType        = left_->valType;
                    
                }
				vector<IRNode> genCode();
};

class IfStatment:public ASTNode{
public:
				ASTNode* cond;
				ASTNode* stmt_list;
				ASTNode* else_stmt_list;
    
				IfStatment(ASTNode* cond_ , ASTNode* stmt_list_, ASTNode* else_stmt_list_){
                    this->cond           = cond_;
                    this->stmt_list      = stmt_list_;
                    this->else_stmt_list = else_stmt_list_;
                    this->nodeType       = ASTNodeType::IFSTMT;
                }
				vector<IRNode> genCode();
};

class ForStatment:public ASTNode{
public:
				ASTNode* initialStatment;//it will be assign statement
				ASTNode* cond; //cond expression
				ASTNode* incrementStatment;//it will be assign statement
				ASTNode* body;//statmrentList
    
				ForStatment(ASTNode * initialStatment_ , ASTNode * cond_, ASTNode * incrementStatment_,ASTNode * body_){
                    this->cond              = cond_;
                    this->initialStatment   = initialStatment_;
                    this->incrementStatment = incrementStatment_;
                    this->body              = body_;
                    this->nodeType          = ASTNodeType::FORSTMT;
                }
				vector<IRNode> genCode();
};

class LoopControl:public ASTNode{
public:
				bool isBreak;
    
				LoopControl(bool isBreak_){
                    isBreak=isBreak_;
                }
				vector<IRNode> genCode();
};

class FuncCall :public ASTNode{
public:
    
    string functionName;
    vector<ASTNode* > parametersExperssions;
    Scope* scope;
    
				FuncCall(string functionName_, vector<ASTNode* > parametersExperssions_ ){
                    parametersExperssions=parametersExperssions_;
                    functionName=functionName_;
                    scope=Scope::getFunctionScope(functionName);
                    this->valType=scope->retValType;
                };
				vector<IRNode> genCode();
};

class Return :public ASTNode{
public:
    
    Scope * functionScope;
    Return(ASTNode * retExp, Scope* currentScope){
        left=retExp;
        functionScope=Scope::getParentFunction(currentScope);
    }
    vector<IRNode> genCode();
};

#endif
