#include "optimizer.h"
//*************  optimizations**************//
//reduce redundant memory access
void Optimizer::updateDefName(vector<IRNode>& instructionList, int starting_index , string newDefName, string oldDefName){

		for(int i=starting_index; i<instructionList.size(); i++){

				if(instructionList[i].op1.compare(oldDefName)==0 )
						instructionList[i].op1=newDefName;

				if(instructionList[i].op2.compare(oldDefName)==0 )
						instructionList[i].op2=newDefName;
				//the idea here is if i reach a label then there is a possible Jump in at this point so this possible [JMP in] so the code after this point might not hold
				//there is no need to do stop of we found a jump because the code after the jump is only accessed while the equality is correct unless a label exists before that
				if(instructionList[i].opcode==Opcode::LABEL ||instructionList[i].getDef().compare(oldDefName)==0||instructionList[i].getDef().compare(newDefName)==0){
						return;
				}
		}

}
void Optimizer::reduceRedundantMoves(vector<IRNode>& instructionList){

		for(int i=0;i<instructionList.size(); i++ ){
				IRNode ir=instructionList[i];
				switch(ir.opcode){
						case STOREF:
						case STOREI:
								Optimizer::updateDefName(instructionList, i+1,ir.op1, ir.op3) ;
								break;
						default:
								break;
				}

		}
}

void Optimizer::removeRepeatedExpression(vector<IRNode>& instructionList){
		map<string, map<string ,map<string ,string> > > expList;
		for(int i=0;i<instructionList.size(); i++ ){
				IRNode & ir=instructionList[i]; //this must be  reference it couses lot bugs for me :(
				switch(ir.opcode){
						case ADDF:
						case ADDI:
						case MULTF:
						case MULTI:{
										   if( expList[ir.op1][ir.op2][getIROpcodeString(ir.opcode)].compare("")!=0  ){
												   Optimizer::updateDefName(instructionList, i+1, expList[ir.op1][ir.op2][getIROpcodeString(ir.opcode)], ir.op3);
												   if(ir.opcode==ADDF||ir.opcode== MULTF)
														   ir.opcode=STOREF;
												   else
														   ir.opcode=STOREI;

												   ir.op1=expList[ir.op1][ir.op2][getIROpcodeString(ir.opcode)];
												   ir.op2="";

										   }
										   else{
												   expList[ir.op1][ir.op2][getIROpcodeString(ir.opcode)]= ir.op3;
												   expList[ir.op2][ir.op1][getIROpcodeString(ir.opcode)]= ir.op3;
										   }
								   }
								   break;
						case SUBI:
						case SUBF:
						case DIVI:
						case DIVF:{
										  if( expList[ir.op1][ir.op2][getIROpcodeString(ir.opcode)].compare("")!=0  ){
												  Optimizer::updateDefName(instructionList, i+1,expList[ir.op1][ir.op2][getIROpcodeString(ir.opcode)], ir.op3);
												  if(ir.opcode==SUBF||ir.opcode==DIVF)
														  ir.opcode=STOREF;
												  else
														  ir.opcode=STOREI;

												  ir.op1=expList[ir.op1][ir.op2][getIROpcodeString(ir.opcode)];
												  ir.op2="";

										  }
										  else{
												  expList[ir.op1][ir.op2][getIROpcodeString(ir.opcode)]= ir.op3;
										  }
								  }
								  break;
						case LABEL:// no need to do the same for the jump
								  expList.clear();
								  break;
						default:
								  break;
				}
				string def=ir.getDef();

				if(def.compare("")!=0)
				{
						expList[def].clear();
						for(map<string, map<string ,map<string ,string> > >::iterator it=expList.begin(); it!=expList.end(); it++)
						{
								it->second[def].clear();
						}
				}
		}
}

void Optimizer::evaluateConstantExpression(vector<IRNode>& instructionList){
		for(int i=0;i<instructionList.size(); i++ ){
				IRNode ir=instructionList[i];
				switch(ir.opcode){
						case ADDI:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM){
										string res=std::to_string(static_cast<long long>( atoi(ir.op1.c_str() )+atoi(ir.op2.c_str())));
										Optimizer::updateDefName(instructionList, i+1, res, ir.op3);
								}
								break;
						case MULTI:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM){
										string res=std::to_string(static_cast<long long>( atoi(ir.op1.c_str())*atoi(ir.op2.c_str())  ));
										Optimizer::updateDefName(instructionList, i+1, res, ir.op3);
								}
								break;
						case SUBI:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM){
										string res=std::to_string(static_cast<long long>(atoi(ir.op1.c_str())-atoi(ir.op2.c_str())));
										Optimizer::updateDefName(instructionList, i+1, res, ir.op3);
								}
								break;
						case DIVI:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM){
										string res=std::to_string(static_cast<long long>(atoi(ir.op1.c_str())/atoi(ir.op2.c_str())));
										Optimizer::updateDefName(instructionList, i+1, res, ir.op3);
								}
								break;
						default:
								break;
				}

		}
}
void Optimizer::removeRedundantIR(vector<IRNode>& in){
		while(true){
				bool reduced=false;

				map<string, int> labelToIndex;
				for(int i=0; i<in.size(); i++){
						if(in[i].opcode==Opcode::LABEL){
								labelToIndex[in[i].op3]=i;
						}
				}
				vector<bool > visited(in.size(), 0);

				for(int i=0; i<in.size(); i++){

						if(in[i].opcode!=Opcode::READF && in[i].opcode!=Opcode::READI && in[i].getDef().size()!=0){//eventhough a read might be redundent we must do it

								string def=in[i].getDef();

								fill(visited.begin(),visited.end(), false);

								if(Optimizer::isUsedLater(def, i+1 ,in , visited,labelToIndex)){
										continue;
								}
								else{
										in.erase(in.begin()+i);
										reduced=true;
										break;
								}
						}
				}
				if (reduced)
						continue;
				else
						return;
		}
}


bool Optimizer::isUsedLater(string& def, int index , vector<IRNode>& instructionList,vector<bool>& visited,	 map<string, int>& labelToIndex){
		//path ended
		if(index>=instructionList.size())
				return false;
		//we have been here and the result was false because if it was true we would have returned and stopped
		if(visited[index]==true)
				return false;

		visited[index]=true;

		//check if  a use
		if(instructionList[index].isAUse(def))
				return true;

		//if is rewritten stop the path
		else if(instructionList[index].isDef(def))
				return false;

		else
		{
				// we need to check next instructions
				//jump case (it has no def but can be a use
				if(instructionList[index].opcode==Opcode::JUMP ){
						// continue checking from the next Instruction if no
						return isUsedLater(def, labelToIndex[instructionList[index].op3] ,instructionList, visited, labelToIndex);
				}
				//conditional jump
				else if(instructionList[index].opcode==Opcode::GT||
						instructionList[index].opcode==Opcode::GE||
						instructionList[index].opcode==Opcode::LT||
						instructionList[index].opcode==Opcode::LE||
						instructionList[index].opcode==Opcode::EQ||
						instructionList[index].opcode==Opcode::NE){
						//if used at any branch then return it is used
						return isUsedLater(def, labelToIndex[instructionList[index].op3],instructionList, visited, labelToIndex) || isUsedLater(def, index+1,instructionList, visited, labelToIndex);
				}
				//not jump case
				else{
						return isUsedLater(def, index+1,instructionList, visited, labelToIndex);
				}

		}

}
void Optimizer::optimizeInstructionSelection(vector<IRNode>& in){
		for(int i=0; i<in.size(); i++){
				if(in[i].opcode==Opcode::ADDI){
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
void Optimizer::removeRedundantLables(vector<IRNode>& instructionList){
		set<string> usedLabels;
		for(int i=0; i<instructionList.size(); i++)
		{
				if(instructionList[i].opcode==Opcode::GT||
				   instructionList[i].opcode==Opcode::GE||
				   instructionList[i].opcode==Opcode::LT||
				   instructionList[i].opcode==Opcode::LE||
				   instructionList[i].opcode==Opcode::EQ||
				   instructionList[i].opcode==Opcode::NE||
				   instructionList[i].opcode==Opcode::JUMP){
						usedLabels.insert(instructionList[i].op3);
				}
		}

		for(int i=0; i<instructionList.size(); i++)
		{
				if(	instructionList[i].opcode==Opcode::LABEL && usedLabels.find(instructionList[i].op3)==usedLabels.end())
				{
						instructionList.erase(instructionList.begin()+i);
						i--;
				}
		}
}
void Optimizer::removeRedundantJumps(vector<IRNode>& instructionList){
		map<string, int> labelToIndex;
		for(int i=0; i<instructionList.size(); i++){
				if(instructionList[i].opcode==Opcode::LABEL){
						labelToIndex[instructionList[i].op3]=i;
				}
		}

		vector<int> toDelete;
		for(int i=0; i<instructionList.size(); i++)
		{
				if( instructionList[i].opcode==Opcode::JUMP && labelToIndex[instructionList[i].op3]==i+1){
						toDelete.push_back(i);
				}
		}
		for(int i=0; i<toDelete.size(); i++){
				instructionList.erase(instructionList.begin()+toDelete[i]);
		}
}
void Optimizer::removeUnReachableCode(vector<IRNode>& instructionList){
		vector<pair<int , int> > toDelete;
		bool isChanged=true;
		while(isChanged){
				isChanged=false;
				int start=-1;
				int end=-1;

				for(int i=0; i<instructionList.size(); i++)
				{
						if( instructionList[i].opcode==Opcode::JUMP )
						{
								start=end=i;
								for(int j=start+1 ; j<instructionList.size(); j++){
										if(instructionList[j].opcode != Opcode::LABEL){
												end++;
										}
										else{
												break;
										}
								}
								if(start==end){
										continue;
								}
								else
										break;
						}
				}
				if(start!=end){
						isChanged=true;
						for(int i=start+1;i<=end; i++)
						{
								instructionList.erase(instructionList.begin()+start+1);
						}
				}
		}
}
void Optimizer::evaluateConstantJumps(vector<IRNode>& instructionList){
		for(int i=0;i<instructionList.size(); i++ ){
				IRNode  ir=instructionList[i];
				switch(ir.opcode){
						case GE:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM && ir.isIntCompare){

										if((atoi(ir.op1.c_str()) >=atoi(ir.op2.c_str()))){
												instructionList[i].opcode=Opcode::JUMP;
												instructionList[i].op1="";
												instructionList[i].op2="";
										}
										else{
												//delete the jump go to next IR
												instructionList.erase(instructionList.begin()+i);
												i--;
										}
								}
								break;
						case GT:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM && ir.isIntCompare){

										if((atoi(ir.op1.c_str()) >atoi(ir.op2.c_str()))){
												instructionList[i].opcode=Opcode::JUMP;
												instructionList[i].op1="";
												instructionList[i].op2="";
										}
										else{
												//delete the jump go to next IR
												instructionList.erase(instructionList.begin()+i);
												i--;
										}
								}
								break;
						case LE:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM && ir.isIntCompare ) {

										if((atoi(ir.op1.c_str()) <=atoi(ir.op2.c_str()))){
												instructionList[i].opcode=Opcode::JUMP;
												instructionList[i].op1="";
												instructionList[i].op2="";
										}
										else{
												//delete the jump go to next IR
												instructionList.erase(instructionList.begin()+i);
												i--;
										}
								}
								break;
						case LT:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM && ir.isIntCompare){

										if((atoi(ir.op1.c_str()) <atoi(ir.op2.c_str()))){
												instructionList[i].opcode=Opcode::JUMP;
												instructionList[i].op1="";
												instructionList[i].op2="";
										}
										else{
												//delete the jump go to next IR
												instructionList.erase(instructionList.begin()+i);
												i--;
										}
								}
								break;
						case EQ:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM && ir.isIntCompare){

										if((atoi(ir.op1.c_str()) ==atoi(ir.op2.c_str()))){
												instructionList[i].opcode=Opcode::JUMP;
												instructionList[i].op1="";
												instructionList[i].op2="";
										}
										else{
												//delete the jump go to next IR
												instructionList.erase(instructionList.begin()+i);
												i--;
										}
								}
								break;
						case NE:
								if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM && ir.isIntCompare){

										if((atoi(ir.op1.c_str()) !=atoi(ir.op2.c_str()))){
												instructionList[i].opcode=Opcode::JUMP;
												instructionList[i].op1="";
												instructionList[i].op2="";
										}
										else{
												//delete the jump go to next IR
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
void Optimizer::optimizeAll(vector<IRNode>& in){
		int oldSize;
		do{
				oldSize=in.size();
				
				Optimizer:: removeRedundantIR(in);
				
				Optimizer::removeRepeatedExpression(in);
				Optimizer:: removeRedundantIR(in);


				Optimizer::reduceRedundantMoves(in);
				Optimizer::removeRedundantIR(in);


				Optimizer::evaluateConstantExpression(in);
				Optimizer::removeRedundantIR(in);


				Optimizer::optimizeInstructionSelection(in);
				Optimizer::removeRedundantIR(in);

				Optimizer::evaluateConstantJumps(in);
				Optimizer::removeUnReachableCode(in);
				Optimizer::removeRedundantJumps(in);
				Optimizer::removeRedundantLables(in);

		}while(oldSize!=in.size());
}
