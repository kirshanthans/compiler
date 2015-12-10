#ifndef CODEGEN_H_
#define CODEGEN_H_
#include <string>
#include <iostream>
#include <sstream>
#include <queue>
#include "astnode.h"
#include "symboltable.h"
#include "optimizer.h"
#include "register.h"

class CodeGen{

		public:
				static map<string, vector<IRNode> > codeByfuncName;
				static map<string, vector<AssemblyNode> > asmByFuncName;
				static map<string, vector<AssemblyNode> > asmByFuncNameUnopt;

				static map<string, vector<IRNode> > codeByfuncNameUnopt;
				static map<string, vector<string> > callgraph;
				static vector<IRNode> genarateCodeForFunction(ASTNode * nodeTree, string functionName);
				static pair<vector<IRNode>, vector<AssemblyNode> > genAllCode(bool opt);
				static void genCallGraph();
				static map<int, vector<int> > genControlFlow(vector<IRNode> & ins);
				static map<int, vector<int> > funControlFlow(vector<IRNode> & ins);
				static map<int, vector<int> > backControlFlow(map<int, vector<int> > & cfg);
				static void printCallGraph();
				static void printControlFlow(map<int, vector<int> > cfg, bool isForward);
				static void printFunControlFlow(bool isForward);
				static void computeLiveOut(vector<IRNode> & InList);
				static void printLiveInOut(vector<IRNode> & InList);
				static void genAssembly();
				static vector<AssemblyNode>  generateAssembleDeclarations();

};

#endif
/* CODEGEN_H_ */
