#ifndef CODEGEN_H_
#define CODEGEN_H_
#include "astnode.h"
#include "symboltable.h"
#include <string>

class CodeGen{

		public:
				static map<string , vector<IRNode> > codeByfuncName; 
				static vector<IRNode> genarateCodeForFunction(ASTNode * nodeTree, string functionName);
				static vector<IRNode> genAllCode();

};

#endif
/* CODEGEN_H_ */
