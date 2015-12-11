#include "codegen.h"
#include "symboltable.h"


map<string, vector<IRNode> > CodeGen::codeByfuncName = map<string, vector<IRNode> >();
map<string, vector<IRNode> > CodeGen::codeByfuncNameUnopt = map<string, vector<IRNode> >();
map<string, vector<string> > CodeGen::callgraph = map<string, vector<string> >();
map<string, vector<AssemblyNode> > CodeGen::asmByFuncName=map<string , vector<AssemblyNode > > ();
map<string, vector<AssemblyNode> > CodeGen::asmByFuncNameUnopt=map<string , vector<AssemblyNode > > ();
vector<AssemblyNode> CodeGen::decls = vector<AssemblyNode>();

vector<IRNode> CodeGen::genarateCodeForFunction(ASTNode * astRoot, string functionName){
		vector<IRNode> code;
		Scope * functionScope=Scope::getFunctionScope(functionName);
		Scope::evaluateLocalOffsets(functionScope);

		vector<AssemblyNode> strDecls = declAssembly(functionScope);
		decls.insert(decls.end(), strDecls.begin(), strDecls.end());
		//add the label
		code.push_back(IRNode(Opcode::LABEL , "", "", "FUNC_"+functionName+"_L"));

		//add the link command
		ostringstream blockSize;
		blockSize<<functionScope->localBlockSize;
		code.push_back(IRNode(Opcode::LINK,"","",blockSize.str()));
		vector<IRNode> funcBody = astRoot->genCode();

		//add function body
		code.insert(code.end(), funcBody.begin(), funcBody.end());
		if(code[code.size()-1].opcode != Opcode::RET){
				code.push_back(IRNode(Opcode::UNLINK,"","",""));
				code.push_back(IRNode(Opcode::RET,"","",""));
		}


		//compue reference for local variables and return values
		string retOffset;
		if(functionScope->retValType!= SymbolEntryType::VOID ){
				ostringstream ss;
				ss<<"$"<<functionScope->retOffset;
				retOffset=ss.str();
		}else{
				retOffset="";
		}


		vector<IRNode> codeCopyToBeOptimized  =  code;

		//compute live in and live out for unoptimized code.
		CodeGen::computeLiveOut(code);
		//CodeGen::printLiveInOut(code);

		//compute assembly for unoptimized code
		RegisterAllocator regAllocUnopt;
		vector<AssemblyNode> assemblyCodeUnopt=regAllocUnopt.genAllocate(code);

		//store assembly and function
		asmByFuncNameUnopt[functionName]=assemblyCodeUnopt;
		codeByfuncNameUnopt[functionName] = code;

		//************************************************************************
		Optimizer::optimizeAllLocal(codeCopyToBeOptimized, retOffset);

		CodeGen::computeLiveOut(codeCopyToBeOptimized);
		//CodeGen::printLiveInOut(codeCopyToBeOptimized);


		RegisterAllocator regAlloc;
		vector<AssemblyNode> assemblyCodeOptimized=regAlloc.genAllocate(codeCopyToBeOptimized);

		asmByFuncName[functionName]=assemblyCodeOptimized;
		codeByfuncName[functionName]=code;

		return code;

}

pair<vector<IRNode>, vector<AssemblyNode> >  CodeGen::genAllCode(bool opt){
		vector<IRNode> code;
		vector<AssemblyNode> assembly;
		string mainName="id_main";
		Scope* mainScope=Scope::getFunctionScope(mainName);

		if(mainScope->retValType!=SymbolEntryType::VOID){
				code.push_back(IRNode(Opcode::PUSH,"","",""));
				code.push_back(IRNode(Opcode::PUSH,"","",""));
				code.push_back(IRNode(Opcode::PUSH,"","",""));
				code.push_back(IRNode(Opcode::PUSH,"","",""));
				code.push_back(IRNode(Opcode::PUSH,"","",""));

				assembly.push_back(AssemblyNode(AssemblyIns::ASM_PUSH,"",""));
				assembly.push_back(AssemblyNode(AssemblyIns::ASM_PUSH,"",""));
				assembly.push_back(AssemblyNode(AssemblyIns::ASM_PUSH,"",""));
				assembly.push_back(AssemblyNode(AssemblyIns::ASM_PUSH,"",""));
				assembly.push_back(AssemblyNode(AssemblyIns::ASM_PUSH,"",""));


		}
		string label="FUNC_id_main_L";
		code.push_back(IRNode(Opcode::JSR,"","",label));
		assembly.push_back(AssemblyNode(AssemblyIns::ASM_JSR,"",label));

		code.push_back(IRNode(Opcode::HALT,"", "",""));
		assembly.push_back(AssemblyNode(AssemblyIns::ASM_HALT,"",""));



		if(opt){
				for(map<string, vector<IRNode> >::iterator it = codeByfuncName.begin(); it != codeByfuncName.end(); it++) {
						vector<IRNode> tmp = it->second;
						code.insert(code.end(), tmp.begin(), tmp.end());
						assembly.insert(assembly.end(),asmByFuncName[it->first].begin(), asmByFuncName[it->first].end());
				}
				//   Optimizer::optimizeAllGlobal(code);
		}
		else{
				for(map<string, vector<IRNode> >::iterator it = codeByfuncNameUnopt.begin(); it != codeByfuncNameUnopt.end(); it++) {
						vector<IRNode> tmp = it->second;
						code.insert(code.end(), tmp.begin(), tmp.end());
						assembly.insert(assembly.end(),asmByFuncNameUnopt[it->first].begin(), asmByFuncNameUnopt[it->first].end());

				}
		}
		return make_pair(code,assembly );
}

void CodeGen::genCallGraph(){
		string caller, callee;
		vector<IRNode> code;
		for(map<string, vector<IRNode> >::iterator it = codeByfuncName.begin(); it != codeByfuncName.end(); ++it){
				caller = it->first;
				code   = it->second;
				for(int i = 0; i < code.size(); ++i){
						if(code[i].opcode == Opcode::JSR){
								callee = code[i].op3.substr(5, code[i].op3.size()-7);
								callgraph[caller].push_back(callee);
						}
				}

		}
}

map<int, vector<int> > CodeGen::genControlFlow(vector<IRNode> & ins){
		map<int, vector<int> > cfg;
		map<string, int> label;
		map<string, vector<int> > callNext;//return posiitons
		//getting labels and calls
		for(int i = 0; i < ins.size(); ++i){
				if(ins[i].opcode == Opcode::LABEL){
						label[ins[i].op3] = i;
				}
				else if(ins[i].opcode == Opcode::JSR){
						callNext[ins[i].op3].push_back(i+1);
				}

		}
		//getting other instructions
		string currentFunc;
		for(int i = 0; i < ins.size(); ++i){
				if(ins[i].opcode == Opcode::LABEL && ins[i].op3.substr(0, 4).compare("FUNC") == 0){
						currentFunc = ins[i].op3;
				}
				if(ins[i].opcode == Opcode::JSR || ins[i].opcode == Opcode::JUMP){
						cfg[i].push_back(label[ins[i].op3]);
						continue;
				}
				else if(ins[i].opcode == Opcode::GT || ins[i].opcode == Opcode::GE || ins[i].opcode == Opcode::LT || ins[i].opcode == Opcode::LE || ins[i].opcode == Opcode::EQ || ins[i].opcode == Opcode::NE){
						cfg[i].push_back(label[ins[i].op3]);
				}
				else if(ins[i].opcode == Opcode::RET){
						cfg[i].insert(cfg[i].end(), callNext[currentFunc].begin(), callNext[currentFunc].end());
						continue;
				}
				if(ins[i].opcode != Opcode::HALT){
						cfg[i].push_back(i+1);
				}
		}

		return cfg;
}

map<int, vector<int> > CodeGen::funControlFlow(vector<IRNode> & ins){
		map<int, vector<int> > cfg;
		map<string, int> label;
		//getting labels and calls
		for(int i = 0; i < ins.size(); ++i){
				if(ins[i].opcode == Opcode::LABEL){
						label[ins[i].op3] = i;
				}
		}
		//getting other instructions
		for(int i = 0; i < ins.size(); ++i){
				if(ins[i].opcode == Opcode::JUMP){
						cfg[i].push_back(label[ins[i].op3]);
						continue;
				}
				else if(ins[i].opcode == Opcode::GT || ins[i].opcode == Opcode::GE || ins[i].opcode == Opcode::LT || ins[i].opcode == Opcode::LE || ins[i].opcode == Opcode::EQ || ins[i].opcode == Opcode::NE){
						cfg[i].push_back(label[ins[i].op3]);
				}
				else if(ins[i].opcode == Opcode::RET){
						continue;
				}
				if(ins[i].opcode != Opcode::HALT){
						cfg[i].push_back(i+1);
				}
		}

		return cfg;
}

map<int, vector<int> > CodeGen::backControlFlow(map<int, vector<int> > & cfg){
		map<int, vector<int> > back_cfg;
		for(map<int, vector<int> >::iterator it = cfg.begin(); it != cfg.end(); ++it){
				for(int i = 0; i < it->second.size(); ++i){
						back_cfg[it->second[i]].push_back(it->first);
				}
		}
		return back_cfg;
}

void CodeGen::printCallGraph(){
		cout << ";*****Call Graph*****" << endl;
		for(map<string, vector<string> >::iterator it = callgraph.begin(); it != callgraph.end(); ++it){
				cout << ";Caller: " << it->first << endl;
				cout << ";Callee: ";
				for(int i = 0; i < it->second.size(); ++i){
						cout << it->second[i];
						cout << " ";
				}
				cout << endl << endl;
		}
}

void CodeGen::printControlFlow(map<int, vector<int> > cfg, bool isForward){
		cout << ";*****Control Flow Graph*****" << endl;
		for(map<int, vector<int> >::iterator it = cfg.begin(); it != cfg.end(); ++it){
				cout << ";Ins #: " << it->first << endl;
				if(isForward)
						cout << ";Successor Ins #: ";
				else
						cout << ";Predecessor Ins #: ";
				for(int i = 0; i < it->second.size(); ++i){
						cout << it->second[i];
						cout << " ";
				}
				cout << endl;
		}
		cout << endl;
}

void CodeGen::printFunControlFlow(bool isForward){
		for(map<string, vector<IRNode> >::iterator itf = codeByfuncName.begin(); itf != codeByfuncName.end(); ++itf){
				cout << ";***** " << itf->first << " Control Flow Graph*****" << endl;

				map<int, vector<int> > cfg;
				if(isForward){
						cfg = funControlFlow(itf->second);
				}
				else{
						cfg = funControlFlow(itf->second);
						cfg = backControlFlow(cfg);
				}

				for(map<int, vector<int> >::iterator it = cfg.begin(); it != cfg.end(); ++it){
						cout << ";Ins #: " << it->first << endl;
						if(isForward)
								cout << ";Successor Ins #: ";
						else
								cout << ";Predecessor Ins #: ";
						for(int i = 0; i < it->second.size(); ++i){
								cout << it->second[i];
								cout << " ";
						}
						cout << endl;
				}
				cout << endl;
		}
}
void CodeGen::computeLiveOut(vector<IRNode> & InList){

		set<string> globalDecs;
		map<int, vector<int> > successorList = CodeGen::funControlFlow(InList);
		map<int, vector<int> > predecessorList = CodeGen::backControlFlow(successorList);

		queue<int> q;

		for(int i=0; i<Scope::globalScope->orderedEntries.size(); i++) {
				if(Scope::globalScope->orderedEntries[i].entryType==SymbolEntryType::FLOAT||
								Scope::globalScope->orderedEntries[i].entryType==SymbolEntryType::INT  ||
								Scope::globalScope->orderedEntries[i].entryType==SymbolEntryType::STRING){
						globalDecs.insert(Scope::globalScope->orderedEntries[i].id);
				}
		}

		//live out of any return is all the global variables
		for(int i=0; i<InList.size();i++){
				if(InList[i].opcode==Opcode::RET){
						InList[i].liveOut.insert(globalDecs.begin(),globalDecs.end());
				}
		}
		//add all instructions to queue
		for(int i=0; i<InList.size(); i++){
				q.push(i);
		}
		while(!q.empty()){
				for(int i=0; i<successorList[q.front()].size(); i++){

						InList[q.front()].liveOut.insert( InList[successorList[q.front()][i]].liveIn.begin(),  InList[successorList[q.front()][i]].liveIn.end()   );
				}

				int oldLiveInSize=InList[q.front()].liveIn.size();

				InList[q.front()].liveIn.insert(InList[q.front()].liveOut.begin(), InList[q.front()].liveOut.end());

				//remove killed variable
				string def=InList[q.front()].getDef();
				InList[q.front()].liveIn.erase(def);

				//add used GENS
				switch (InList[q.front()].opcode) {
						case Opcode::ADDI:
						case Opcode::ADDF:
						case Opcode::SUBI:
						case Opcode::SUBF:
						case Opcode::MULTI:
						case Opcode::MULTF:
						case Opcode::DIVI:
						case Opcode::DIVF:
						case Opcode::GT:
						case Opcode::GE:
						case Opcode::LT:
						case Opcode::LE:
						case Opcode::NE:
						case Opcode::EQ:

								if(findOperandType(InList[q.front()].op1)==OperandType::MEM||
												findOperandType(InList[q.front()].op1)==OperandType::TEMP)
										InList[q.front()].liveIn.insert(InList[q.front()].op1);

								if(findOperandType(InList[q.front()].op2)==OperandType::MEM ||
												findOperandType(InList[q.front()].op2)==OperandType::TEMP)
										InList[q.front()].liveIn.insert(InList[q.front()].op2);


								break;
						case Opcode::STOREI:
						case Opcode::STOREF:
								if(findOperandType(InList[q.front()].op1)==OperandType::MEM||
												findOperandType(InList[q.front()].op1)==OperandType::TEMP)
										InList[q.front()].liveIn.insert(InList[q.front()].op1);

								break;

						case Opcode::WRITEF:
						case Opcode::WRITEI:
						case Opcode::PUSH:

								if(findOperandType(InList[q.front()].op3)==OperandType::MEM||
												findOperandType(InList[q.front()].op3)==OperandType::TEMP)
										InList[q.front()].liveIn.insert(InList[q.front()].op3);



								break;

						case Opcode::JSR:
								InList[q.front()].liveIn.insert(globalDecs.begin(),globalDecs.end());
								break;

						default:
								break;
				}

				if(oldLiveInSize!= InList[q.front()].liveIn.size()){

						for(int i=0 ; i< predecessorList[q.front()].size(); i++){
								q.push(predecessorList[q.front()][i]);
						}
				}

				q.pop();

		}
}
void  CodeGen::printLiveInOut(vector<IRNode> & InList){
		for(int i=0; i<InList.size(); i++){
				cout<<";"<<InList[i].toIRString()<<":"<<endl;
				cout<<";liveIn:";
				for(set<string>::iterator it=InList[i].liveIn.begin(); it!=InList[i].liveIn.end(); it++){
						cout<< *it<<"--";
				}
				cout<<"\n;liveout:";
				for(set<string>::iterator it=InList[i].liveOut.begin(); it!=InList[i].liveOut.end(); it++){
						cout<< *it<<"--";
				}
				cout<<endl ;
		}
}
vector<AssemblyNode>  CodeGen::generateAssembleDeclarations(){
		//genall glabal decs

		vector<AssemblyNode> globalDecls=declAssembly(Scope::globalScope);
		decls.insert(decls.end(), globalDecls.begin(), globalDecls.end());

		for(set<string>::iterator it= RegisterAllocator::newMemLocations.begin();it!=RegisterAllocator::newMemLocations.end(); it++){
				decls.push_back(AssemblyNode(AssemblyIns::ASM_VAR, "", *it  ));
		}
		return decls;
}

