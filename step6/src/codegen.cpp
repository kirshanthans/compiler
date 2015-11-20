#include <iostream>
#include <sstream>
#include "astnode.h"
#include "codegen.h"


map<string , vector<IRNode> > CodeGen::codeByfuncName = map<string, vector<IRNode> >();


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
		codeByfuncName[functionName]=code;

		//cout << convertCodetoString(code, "", "");
		//cout << "debug " << functionName << endl;
		return code;

}

vector<IRNode> CodeGen::genAllCode(){
		vector<IRNode> code;
		string mainName="main";
		Scope* mainScope=Scope::getFunctionScope("main");

		if(mainScope->retValType!=SymbolEntryType::VOID)
				code.push_back(IRNode(Opcode::PUSH,"","",""));
		string label="FUNC_main_L";
		code.push_back(IRNode(Opcode::JSR,"","",label));
		code.push_back(IRNode(Opcode::HALT,"", "",""));
		
		for(map<string, vector<IRNode> >::iterator it = codeByfuncName.begin(); it != codeByfuncName.end(); it++) {
				vector<IRNode> tmp = it->second;
				code.insert(code.end(), tmp.begin(), tmp.end());
		}

		return code;
}
