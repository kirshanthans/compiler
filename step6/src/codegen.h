#ifndef CODEGEN_H_
#define CODEGEN_H_
#include "astnode.h"
#include "symboltable.h"
#include <string>

class CodeGen{

		public:
				static map<string, vector<IRNode> > codeByfuncName; 
				static map<string, vector<IRNode> > codeByfuncNameUnopt; 
				static map<string, vector<string> > callgraph;
				static vector<IRNode> genarateCodeForFunction(ASTNode * nodeTree, string functionName);
				static vector<IRNode> genAllCode(bool opt);
				static void genCallGraph();
				static void printCallGraph();

};

#endif
/* CODEGEN_H_ */
