#ifndef ASTNODE_H_
#define ASTNODE_H_
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include"symboltable.h"

class AssemblyNode;
class IRNode;
class ASTNode;
class StatementList;
class AssignStatement;
class Identifier;
class Constant;

using namespace std;

enum Opcode      {ADDI = 0, ADDF = 1, SUBI = 2, SUBF = 3, MULTI = 4, MULTF = 5, DIVI = 6, DIVF = 7, STOREI = 8, STOREF = 9, GT = 10, GE = 11, LT = 12, LE = 13, NE = 14, EQ = 15, JUMP = 16, LABEL = 17, READI = 18, READF = 19, WRITEI = 20, WRITEF = 21, WRITES = 22};
enum ASTNodeType {ID = 0, BINOP = 1, STMLIST = 2, CONST = 3, ASSIGN = 4, IO = 5};
enum BinopType   {ADD = 0, SUB = 1, MUL = 2, DIV =3};
enum AssemblyIns {ASM_ADDI = 0, ASM_ADDR = 1, ASM_SUBI = 2, ASM_SUBR = 3, ASM_MULTI = 4, ASM_MULTR = 5, ASM_DIVI = 6, ASM_DIVR = 7, ASM_MOVE = 8, ASM_READI = 9, ASM_READR = 10, ASM_WRITEI = 11, ASM_WRITER = 12, ASM_WRITES = 13, ASM_INCI = 14, ASM_DECI = 15, ASM_CMPI = 16, ASM_CMPR = 17, ASM_PUSH = 18, ASM_POP = 19, ASM_JSR = 20, ASM_RET = 21, ASM_LINK = 22, ASM_UNLINK = 23, ASM_LABEL = 24, ASM_JMP = 25, ASM_JQT = 26, ASM_JLT = 27, ASM_JGE = 28, ASM_JLE = 29, ASM_JEQ = 30, ASM_JNE = 31, ASM_VAR = 32, ASM_STR = 33, ASM_HALT = 34, ASM_END = 35};
enum OperandType {IMM = 0, TEMP = 1, MEM = 2};

string getNextTemp();
string getNextReg();
string getReg(string temp);

string getIROpcodeString(Opcode opcode);
string getAssemblyOpcodeString(AssemblyIns ins);
string convertCodetoString(vector<IRNode> code);
vector<AssemblyNode> declAssembly(Scope* scope);
vector<AssemblyNode> genAssembly(vector<IRNode> code);
string convertAssemblytoString(vector<AssemblyNode> code);

OperandType findOperandType(string op);

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
				IRNode(Opcode opcode_, string op1_, string op2_, string op3_){
						opcode = opcode_;
						op1    = op1_;
						op2    = op2_;
						op3    = op3_;
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

				//opt1
				virtual bool hasImmediateValue(){
						return false;
				}
				virtual ValueHolder getImmediateValue(){
						return immediateVal;
				}

				virtual string getTempVal(){
						return tempVal;
				}
				virtual string getImmediateValueAsString(){
						ostringstream tmp;
						ValueHolder val=getImmediateValue();
						if(valType==FLOAT)
								tmp<<fixed<<val.floatVal;
						else
								tmp<<fixed<<val.intval;

						return tmp.str();
				}

				void setImmediateValue(int immediateVal_){
						immediateVal.intval=immediateVal_;
				}
				void setImmediateValue(float immediateVal_){
						immediateVal.floatVal=immediateVal_;
				}

		protected:
				string tempVal;
				ValueHolder immediateVal;


};

class StatementList:public ASTNode{
		public:

				StatementList(ASTNode* left_, ASTNode* right_){
						left = left_;
						right = right_;
						nodeType=ASTNodeType::STMLIST;
				}
				vector<IRNode> genCode();

};

class AssignStatement:public ASTNode{
		public:

				AssignStatement(ASTNode * left_, ASTNode* right_){
						left = left_;
						right = right_;
						nodeType=ASTNodeType::ASSIGN;
						valType=left_->valType;
				}
				vector<IRNode> genCode();
};

class Identifier:public ASTNode{
		public:
				string name;
				Scope * scope;
				Identifier(string name_, Scope* scope_){
						name = name_;
						scope=scope_;
						nodeType=ASTNodeType::ID;
						valType=scope->getIdType(name);
						tempVal=name_;
				}
				vector<IRNode> genCode();
				bool hasImmediateValue();
				ValueHolder getImmediateValue();
				string getTempVal();


};

class Constant:public ASTNode{
		public:
				string valAsString;
				Constant(string valAsString_,SymbolEntryType  valType_){
						valAsString = valAsString_;
						valType=valType_;
						nodeType=ASTNodeType::CONST;
				}
				vector<IRNode> genCode();
				bool hasImmediateValue();

};

class IOCommand:public ASTNode{
		public:
				bool isRead;
				vector<ASTNode *> idList;
				IOCommand(bool isRead_, vector<ASTNode *> idList_){
						isRead=isRead_;
						nodeType=ASTNodeType::IO;
						idList=idList_;
				}
				vector<IRNode> genCode();

};

class Binop:public ASTNode{
		public:
				BinopType operationType;
				bool valueKnown;
				Binop(ASTNode * left_ , ASTNode* right_, BinopType operationType_){
						left          = left_;
						right         = right_;
						operationType = operationType_;
						nodeType      = ASTNodeType::BINOP;
						valType       = left_->valType;
						valueKnown    = false;
				}
				vector<IRNode> genCode();
				bool hasImmediateValue();
};
#endif
