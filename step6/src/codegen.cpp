#include <iostream>
#include <sstream>
#include "astnode.h"
#include "codegen.h"
#include"optimizer.h"


map<string , vector<IRNode> > CodeGen::codeByfuncName = map<string, vector<IRNode> >();
map<string , vector<IRNode> > CodeGen::codeByfuncNameUnopt = map<string, vector<IRNode> >();


vector<IRNode> CodeGen::genarateCodeForFunction(ASTNode * astRoot, string functionName){
		vector<IRNode> code;
		Scope * functionScope=Scope::getFunctionScope(functionName);
		Scope::evaluateLocalOffsets(functionScope);
		//add the label
		code.push_back(IRNode(Opcode::LABEL , "", "", "FUNC_"+functionName+"_L"));
		//add the link command
		ostringstream blockSize;
		blockSize<<functionScope->localBlockSize;
		code.push_back(IRNode(Opcode::LINK,"","",blockSize.str()));
		vector<IRNode> funcBody = astRoot->genCode();

		code.insert(code.end(), funcBody.begin(), funcBody.end());
		if(code[code.size()-1].opcode != Opcode::RET){
				code.push_back(IRNode(Opcode::UNLINK,"","",""));
				code.push_back(IRNode(Opcode::RET,"","",""));
		}


		string retOffset;
		if(functionScope->retValType!= SymbolEntryType::VOID ){
				ostringstream ss;
				ss<<"$"<<functionScope->retOffset;
				retOffset=ss.str();
		}else{
				retOffset="";
		}
		
		codeByfuncNameUnopt[functionName] = code;

		Optimizer::optimizeAllLocal(code, retOffset);

		codeByfuncName[functionName]=code;

		return code;

}

vector<IRNode> CodeGen::genAllCode(bool opt){
		vector<IRNode> code;
		string mainName="main";
		Scope* mainScope=Scope::getFunctionScope("main");

		if(mainScope->retValType!=SymbolEntryType::VOID)
				code.push_back(IRNode(Opcode::PUSH,"","",""));
		string label="FUNC_main_L";
		code.push_back(IRNode(Opcode::JSR,"","",label));
		code.push_back(IRNode(Opcode::HALT,"", "",""));
		if(opt){
				for(map<string, vector<IRNode> >::iterator it = codeByfuncName.begin(); it != codeByfuncName.end(); it++) {
						vector<IRNode> tmp = it->second;
						code.insert(code.end(), tmp.begin(), tmp.end());
				}
		Optimizer::optimizeAllGlobal(code);
		}
		else{
				for(map<string, vector<IRNode> >::iterator it = codeByfuncNameUnopt.begin(); it != codeByfuncNameUnopt.end(); it++) {
						vector<IRNode> tmp = it->second;
						code.insert(code.end(), tmp.begin(), tmp.end());
				}
		}
		return code;
}
