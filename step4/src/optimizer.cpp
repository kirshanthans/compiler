#include "optimizer.h"
//update name
void Optimizer::updateDefName(vector<IRNode>& instructionList, int starting_index , string newDefName, string oldDefName){
		for(int i=starting_index; i<instructionList.size(); i++){
				if(instructionList[i].op1.compare(oldDefName)==0 )
						instructionList[i].op1=newDefName;

				if(instructionList[i].op2.compare(oldDefName)==0 )
						instructionList[i].op2=newDefName;

		}

}
//remove redundant expresison
void Optimizer::removeRedundantExpression(vector<IRNode>& instructionList){
		map<string, string > expList;

		for(int i=0;i<instructionList.size(); i++ ){
				IRNode ir=instructionList[i];
				switch(ir.opcode){
						case ADDF:
						case ADDI:
						case MULTF:
						case MULTI:
								{
										string str1= getIROpcodeString(ir.opcode)+","+ir.op1+","+ir.op2;
										string str2= getIROpcodeString(ir.opcode)+","+ir.op2+","+ir.op1;
										if(expList.find(str1)!=expList.end()){
												Optimizer::updateDefName(instructionList, i+1, expList[str1], ir.op3);
												instructionList.erase(instructionList.begin()+i);
												i--;
										}
										else{
												expList[str1]= ir.op3;
												expList[str2]= ir.op3;
										}

								}
								break;
						case SUBI:
						case SUBF:
						case DIVI:
						case DIVF:
								{
										string str1= getIROpcodeString(ir.opcode)+","+ir.op1+","+ir.op2;
										if(expList.find(str1)!=expList.end()){
												Optimizer::updateDefName(instructionList, i+1, expList[str1], ir.op3);
												instructionList.erase(instructionList.begin()+i);
												i--;
										}
										else{
												expList[str1]= ir.op3;
										}

								}
						case STOREI:
								{
										if(findOperandType( instructionList[i].op1)==OperandType::TEMP && findOperandType( instructionList[i].op3)==OperandType::TEMP)
										{
												Optimizer::updateDefName(instructionList, i+1, ir.op1, ir.op3);
												instructionList.erase(instructionList.begin()+i);
												i--;
										}

								}
								break;
						default:
								break;
				}

		}
}
//remove reduntant IR ins using use-def chains
void Optimizer::removeRedundantIR(vector<IRNode>& in){
		while(true){
				bool reduced=false;
				for(int i=0; i<in.size(); i++){
						if(in[i].opcode!=Opcode::READF && in[i].opcode!=Opcode::READI && in[i].getDef().size()!=0){//eventhough a read might be redundent we must do it
								string def=in[i].getDef();
								bool removeItem=false;
								int j;
								for( j=i+1;j<in.size(); j++){
										if(in[j].isAUse(def))
												break;
										else if(in[j].isDef(def))
										{
												removeItem=true;
												break;
										}
								}
								if(removeItem || j==in.size()){
										in.erase(in.begin()+i);
										reduced=true;
										break;
								}
								else
										continue;
						}
				}
				if (reduced)
						continue;
				else
						return;
		}
}
//remove reduntant IR ins using use-def chains
void Optimizer::optimizeInstructionSelection(vector<IRNode>& in){
		for(int i=0; i<in.size(); i++){
				if(in[i].opcode==Opcode::ADDI ){
						if(in[i].op1.compare("0")==0 ||in[i].op2.compare("0")==0){
								in[i].opcode=Opcode::STOREI;
								in[i].op1=(in[i].op1.compare("0")==0)? in[i].op2:in[i].op1;
								in[i].op2="";
						}
				}

				else if(in[i].opcode==Opcode::MULTI){
						if(in[i].op1.compare("1")==0 ||in[i].op2.compare("1")==0){
								in[i].opcode=Opcode::STOREI;
								in[i].op1=(in[i].op1.compare("1")==0)? in[i].op2:in[i].op1;
								in[i].op2="";
						}
				}

				else if(in[i].opcode==Opcode::SUBI){
						if(in[i].op2.compare("0")==0){
								in[i].opcode=Opcode::STOREI;
								in[i].op2="";
						}
				}
				else if(in[i].opcode==Opcode::DIVI){
						if(in[i].op2.compare("1")==0){
								in[i].opcode=Opcode::STOREI;
								in[i].op2="";
						}
				}
				else if(in[i].opcode==Opcode::MULTI){
						if(in[i].op1.compare("0")==0 ||in[i].op2.compare("0")==0){
								in[i].opcode=Opcode::STOREI;
								in[i].op1="0";
								in[i].op2="";
						}


				}
		}
}

