#include "register.h"
set<string> RegisterAllocator::newMemLocations=set<string>();

void RegisterAllocator::printRegTable(){
		cout<<";";
		for(int i=0; i<NUMREG; i++){
				cout<<this->regFile[i]<<",";
		}
		cout<<endl;
		cout<<";";

		for(int i=0; i<NUMREG; i++){
				cout<<this->isEmpty[i]<<",";
		}
		cout<<endl;

		cout<<";";
		for(int i=0; i<NUMREG; i++){
				cout<<this->isDirty[i]<<",";
		}
		cout<<endl;
}

string RegisterAllocator::regStr(int i){
		string str_reg[]={"r0", "r1", "r2", "r3"};
		return str_reg[i];
}
bool RegisterAllocator::existsInRegFile(string op){
		for(int i = 0; i < NUMREG; ++i){
				if(!this->isEmpty[i] && this->regFile[i].compare(op) == 0){
						return true;
				}
		}
		//cout<<";"<<op<<"does not exists in reg file for some reason!!!!!"<<endl;

		return false;
}
string RegisterAllocator::getOperandMemLocation(string op){

		if( findOperandType(op)==OperandType::TEMP ){
				return "temp_"+op.substr(1,op.size()-1);

		}
		else
				return op;
}
int RegisterAllocator::ensure(vector<AssemblyNode>& assembly, IRNode ins, string op){

		for(int i = 0; i < NUMREG; ++i){
				//check if already exists
				if(!this->isEmpty[i] && this->regFile[i].compare(op) == 0)
						return i;
		}
		int r = allocate(assembly, ins, op);
		assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, getOperandMemLocation(op), regStr(r)));
		return r;
}


void RegisterAllocator::free(vector<AssemblyNode>& assembly, IRNode ins, int regNum){

		//cout<<";freeing regNum "<<regNum<<endl;
		//this->printRegTable();

		if(this->isDirty[regNum] && ins.liveOut.find(this->regFile[regNum]) != ins.liveOut.end()){
				assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, regStr(regNum), getOperandMemLocation(regFile[regNum])));
				//cout<<";free created this Instruction move from "<<regStr(regNum)<<"into:"<<getOperandMemLocation(regFile[regNum])<<endl;
				if(findOperandType(regFile[regNum])==OperandType::TEMP){
						newMemLocations.insert(getOperandMemLocation(regFile[regNum]));
				}

		}
		this->isEmpty[regNum] = true;
		this->isDirty[regNum] = false;
}

int RegisterAllocator::allocate(vector<AssemblyNode>& assembly,IRNode  ins, string op){


		for(int i=0; i<NUMREG; i++){
				if(regFile[i].compare(op)==0 &&isEmpty[i]==false){
						return i;
				}
		}


		for(int i = 0; i < NUMREG; i++){
				if(this->isEmpty[i]){
						//two lines added
						this->isEmpty[i]=false;
						this->regFile[i]=op;
						return i;

				}
		}
		int j = this->choose(ins,op);
		this->free(assembly, ins, j);
		this->regFile[j] = op;
		this->isEmpty[j] = false;
		return j;

}
int RegisterAllocator::choose(const IRNode & ins, string op){
		bool dontSelect[NUMREG];
		for(int i=0; i<NUMREG; i++){
				if(regFile[i].compare(ins.op1)==0|| regFile[i].compare(ins.op2)==0)
						dontSelect[i]=true;
				else
						dontSelect[i]=false;
		}
		for(int i=0; i<NUMREG; i++){
				if(dontSelect[i]==false &&isDirty[i]==false && ins.liveOut.find(regFile[i])==ins.liveOut.end())
						return i;
		}
		for(int i=0; i<NUMREG; i++){
				if(dontSelect[i]==false &&isDirty[i]==false )
						return i;
		}
		for(int i=0; i<NUMREG; i++){
				if(dontSelect[i]==false)
						return i;
		}
		return 0 ;
}
void RegisterAllocator::resetAndWriteBackEndOfBB(vector<AssemblyNode>& assembly){
		for(int i=0; i<NUMREG;i++){

				//cout<<";reset and writeback everything"<<endl;
				if(isEmpty[i]==false && isDirty[i]==true ){
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, regStr(i), getOperandMemLocation(regFile[i])));

						if(findOperandType(regFile[i])==OperandType::TEMP){
								newMemLocations.insert(getOperandMemLocation(regFile[i]));
						}
				}
				this->regFile[i] = "";
				this->isDirty[i] = false;
				this->isEmpty[i] = true;
		}

}
void RegisterAllocator::writeBackGlobalVariables(vector<AssemblyNode>& assembly){
		for(int i=0; i<NUMREG;i++){

				if(isEmpty[i]==false && isDirty[i]==true && isGlobalVar(regFile[i])){
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, regStr(i), getOperandMemLocation(regFile[i])));

						//cout<<";writing back a global variable before function call:"<<regFile[i]<<endl;
				}

		}

}


vector<AssemblyNode> RegisterAllocator::genAllocate(vector<IRNode> & ins ){
		vector<AssemblyNode> ret;
		//genearte basib blocks end locations in set
		vector<bool> endBasicBlocksLocations(ins.size(), false);

		vector <int> tmp=basicBlocksIndex(ins);
		for(int i=0; i<tmp.size(); i++)
				endBasicBlocksLocations[tmp[i]]=true;


		for(int i=0 ;i <ins.size(); i++ ){

				//if it is a function call write back global variables before calling.
				if(ins[i].opcode==Opcode::JSR)
						writeBackGlobalVariables(ret);

				int opreg1=-1,opreg2=-1,opreg3=-1;
				string opreg1str,opreg2str,opreg3str;

				//insuring inputs
				switch (ins[i].opcode) {
						case Opcode::ADDI:
						case Opcode::ADDF:
						case Opcode::SUBI:
						case Opcode::SUBF:
						case Opcode::MULTI:
						case Opcode::MULTF:
						case Opcode::DIVI:
						case Opcode::DIVF:

								if(findOperandType(ins[i].op1)!=OperandType::IMM){
										opreg1=ensure(ret, ins[i],ins[i].op1);
										opreg1str=regStr(opreg1);
								}
								else
										opreg1str=ins[i].op1;

								if(findOperandType(ins[i].op2)!=OperandType::IMM){
										opreg2=ensure(ret, ins[i],ins[i].op2);
										opreg2str=regStr(opreg2);
								}else
										opreg2str=ins[i].op2;


								break;


						case Opcode::GT:
						case Opcode::GE:
						case Opcode::LT:
						case Opcode::LE:
						case Opcode::NE:
						case Opcode::EQ:
								if(findOperandType(ins[i].op1)!=OperandType::IMM){
										opreg1=ensure(ret, ins[i],ins[i].op1);
										opreg1str=regStr(opreg1);
								}
								else
										opreg1str=ins[i].op1;

								/*if(findOperandType(ins[i].op2)!=OperandType::IMM){*/
								opreg2=ensure(ret, ins[i],ins[i].op2);
								opreg2str=regStr(opreg2);
								/*}else
								  opreg2str=ins[i].op2;
								  */

								break;
						case Opcode::STOREI:
						case Opcode::STOREF:
								if(findOperandType(ins[i].op1)!=OperandType::IMM){
										opreg1=ensure(ret, ins[i],ins[i].op1);
										opreg1str=regStr(opreg1);

								}else
										opreg1str=ins[i].op1;

								break;

						case Opcode::WRITEF:
						case Opcode::WRITEI:
						case Opcode::PUSH:
								if(existsInRegFile(ins[i].op3)){
										opreg3=ensure(ret, ins[i],ins[i].op3);
										opreg3str=regStr(opreg3);

								}else
										opreg3str=ins[i].op3;
								break;
						case Opcode::WRITES:
								opreg3str=ins[i].op3;
								break;

						default:
								break ;
				}


				//cout<<";"<<"before allocation"<<endl;
				//this->printRegTable();

				//allocate place for the result
				switch (ins[i].opcode) {
						case Opcode::ADDI:
						case Opcode::ADDF:
						case Opcode::SUBI:
						case Opcode::SUBF:
						case Opcode::MULTI:
						case Opcode::MULTF:
						case Opcode::DIVI:
						case Opcode::DIVF:
						case Opcode::READI:
						case Opcode::READF:
								opreg3=this->allocate(ret, ins[i],ins[i].op3);
								opreg3str=regStr(opreg3);
								isDirty[opreg3]=true;

								break;

						case Opcode::STOREI:
						case Opcode::STOREF:
								if(ins[i].isStoreRet==false){

										opreg3=this->allocate(ret, ins[i],ins[i].op3);
										opreg3str=regStr(opreg3);
										isDirty[opreg3]=true;

										break;

								}else{
										opreg3str=ins[i].op3;
										break;
								}
								break;
						case Opcode::POP:
								if(ins[i].op3.compare("")!=0){
										opreg3=this->allocate(ret, ins[i],ins[i].op3);
										opreg3str=regStr(opreg3);
										isDirty[opreg3]=true;

								}else{
										opreg3=-1;
										opreg3str="";
								}

								break;
						default:
								break;
				}






				//cout<<";finish allocating"<<endl;
				//  free
				switch (ins[i].opcode) {
						case Opcode::ADDI:
						case Opcode::ADDF:
						case Opcode::SUBI:
						case Opcode::SUBF:
						case Opcode::MULTI:
						case Opcode::MULTF:
						case Opcode::DIVI:
						case Opcode::DIVF:


								if(findOperandType(ins[i].op1)!=OperandType::IMM &&
												ins[i].liveOut.find(ins[i].op1)==ins[i].liveOut.end() && opreg1!=opreg3 )
										this->free(ret,ins[i], opreg1);

								if(findOperandType(ins[i].op2)!=OperandType::IMM &&
												ins[i].liveOut.find(ins[i].op2)==ins[i].liveOut.end() && opreg2!=opreg3)
										this->free(ret,ins[i], opreg2);


								break;




						case Opcode::GT:
						case Opcode::GE:
						case Opcode::LT:
						case Opcode::LE:
						case Opcode::NE:
						case Opcode::EQ:

								if(findOperandType(ins[i].op1)!=OperandType::IMM &&
												ins[i].liveOut.find(ins[i].op1)==ins[i].liveOut.end() && opreg1!=opreg3 )
										this->free(ret,ins[i], opreg1);

								if(/*findOperandType(ins[i].op2)!=OperandType::IMM &&*/
												ins[i].liveOut.find(ins[i].op2)==ins[i].liveOut.end() && opreg2!=opreg3)
										this->free(ret,ins[i], opreg2);


								break;
						case Opcode::STOREI:
						case Opcode::STOREF:

								if(findOperandType(ins[i].op1)!=OperandType::IMM &&
												ins[i].liveOut.find(ins[i].op1)==ins[i].liveOut.end() && opreg1!=opreg3)
										this->free(ret,ins[i], opreg1);


								break;

						case Opcode::WRITEF:
						case Opcode::WRITEI:
						case Opcode::PUSH:

								if(ins[i].liveOut.find(ins[i].op3)==ins[i].liveOut.end() && opreg3!=-1)
										this->free(ret,ins[i], opreg3);

								break;

						default:
								break ;
				}




				IRNode ir=ins[i];

				if(ir.opcode == Opcode::JUMP||ir.opcode == Opcode::GT||ir.opcode == Opcode::GE||ir.opcode == Opcode::LT||ir.opcode == Opcode::LE||ir.opcode == Opcode::NE||ir.opcode == Opcode::EQ|| ir.opcode==Opcode::RET){
						if(endBasicBlocksLocations[i] )
								this->resetAndWriteBackEndOfBB(ret);
				}
				//cout<<";|"<<opreg1<<"|"<<opreg2<<"|"<<opreg3<<endl;
				vector<AssemblyNode> tmp=ins[i].toAssembly(opreg1str,opreg2str,opreg3str);
				ret.insert(ret.end(),tmp.begin(), tmp.end());



				//cout<<";"<<ins[i].toIRString()<<endl;
				//cout<<convertAssemblytoString(tmp, true)<<endl;
				//printRegTable();

				if(!(ir.opcode == Opcode::JUMP||ir.opcode == Opcode::GT||ir.opcode == Opcode::GE||ir.opcode == Opcode::LT||ir.opcode == Opcode::LE||ir.opcode == Opcode::NE||ir.opcode == Opcode::EQ|| ir.opcode==Opcode::RET)){
						if(endBasicBlocksLocations[i] )
								this->resetAndWriteBackEndOfBB(ret);
				}

		}

		return ret;
}




