#include "optimizer.h"
//*************  optimizations**************//
//reduce redundant memory access

bool isGlobalVar(string var) {
    if(findOperandType(var)==OperandType::MEM && var.substr(0,1).compare("$")!=0)
        return true;
    
    return false;
}
void Optimizer::updateDefName(vector<IRNode>& instructionList, int starting_index , string newDefName, string oldDefName){
    
    for(int i=starting_index; i<instructionList.size(); i++){
        
        if(instructionList[i].op1.compare(oldDefName)==0 )
            instructionList[i].op1=newDefName;
        
        if(instructionList[i].op2.compare(oldDefName)==0 )
            instructionList[i].op2=newDefName;
        
        
        if(instructionList[i].opcode==Opcode::LABEL ||
           instructionList[i].getDef().compare(oldDefName)==0||
           instructionList[i].getDef().compare(newDefName)==0||
           (    instructionList[i].opcode==Opcode::JSR
            && ( isGlobalVar(oldDefName) || isGlobalVar(newDefName) )
            )
           )
            return;
        
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
                
            case  JSR:
            {
                //remove any exp where one of the defs is global
                bool isChanged=true;
                
                while(isChanged){
                    isChanged=false;
                    for(map<string, map<string ,map<string ,string> > >::iterator it=expList.begin(); it!=expList.end(); it++){
                        
                        if(isGlobalVar(it->first)){
                            expList.erase(it);
                            isChanged=true;
                            break;
                        }
                    }
                    
                }
                
                for(map<string, map<string ,map<string ,string> > >::iterator it=expList.begin(); it!=expList.end(); it++){
                    
                    bool isChanged=true;
                    while(isChanged){
                        isChanged=false;
                        for( map<string ,map<string ,string> > ::iterator it2=it->second.begin(); it2!=it->second.end(); it2++){
                            
                            if(isGlobalVar(it2->first)){
                                it->second.erase(it2);
                                isChanged=true;
                                break;
                            }
                        }
                    }
                    
                }
            }
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

void Optimizer::updateDefNameConstExp(vector<IRNode>& instructionList, int starting_index , string newDefName, string oldDefName){
    
    for(int i=starting_index; i<instructionList.size(); i++){
        
        if(instructionList[i].op1.compare(oldDefName)==0 )
            instructionList[i].op1=newDefName;
        
        if(instructionList[i].op2.compare(oldDefName)==0 )
            instructionList[i].op2=newDefName;
        //the idea here is if i reach a label then there is a possible Jump in at this point so this possible [JMP in] so the code after this point might not hold
        //there is no need to do stop of we found a jump because the code after the jump is only accessed while the equality is correct unless a label exists before that
        /*if(instructionList[i].opcode==Opcode::LABEL ||instructionList[i].getDef().compare(oldDefName)==0||instructionList[i].getDef().compare(newDefName)==0){
         return;*/
    }
}




//local f
void Optimizer::evaluateConstantExpression(vector<IRNode>& instructionList){
    for(int i=0;i<instructionList.size(); i++ ){
        IRNode ir=instructionList[i];
        switch(ir.opcode){
            case ADDI:
                if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM){
                    string res=std::to_string(static_cast<long long>( atoi(ir.op1.c_str() )+atoi(ir.op2.c_str())));
                    Optimizer::updateDefNameConstExp(instructionList, i+1, res, ir.op3);
                }
                break;
            case MULTI:
                if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM){
                    string res=std::to_string(static_cast<long long>( atoi(ir.op1.c_str())*atoi(ir.op2.c_str())  ));
                    Optimizer::updateDefNameConstExp(instructionList, i+1, res, ir.op3);
                }
                break;
            case SUBI:
                if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM){
                    string res=std::to_string(static_cast<long long>(atoi(ir.op1.c_str())-atoi(ir.op2.c_str())));
                    Optimizer::updateDefNameConstExp(instructionList, i+1, res, ir.op3);
                }
                break;
            case DIVI:
                if(findOperandType(ir.op1)==OperandType::IMM && findOperandType(ir.op2)==OperandType::IMM){
                    string res=std::to_string(static_cast<long long>(atoi(ir.op1.c_str())/atoi(ir.op2.c_str())));
                    Optimizer::updateDefNameConstExp(instructionList, i+1, res, ir.op3);
                }
                break;
            default:
                break;
        }
        
    }
}



//ver 1 per global
void Optimizer::removeRedundantIRGlobal(vector<IRNode>& in){
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
            
            if(in[i].opcode==Opcode::ADDI   ||
               in[i].opcode==Opcode::ADDF   ||
               in[i].opcode==Opcode::SUBI   ||
               in[i].opcode==Opcode::SUBF   ||
               in[i].opcode==Opcode::MULTI  ||
               in[i].opcode==Opcode::MULTF  ||
               in[i].opcode==Opcode::DIVI   ||
               in[i].opcode==Opcode::DIVF   ||
               in[i].opcode==Opcode::STOREF ||
               in[i].opcode==Opcode::STOREI )
            {
                
                string def=in[i].getDef();
                fill(visited.begin(),visited.end(), false);
                if(!isGlobalVar(def))
                    continue;
                
                if(Optimizer::isUsedLaterGlobal(def, i+1 ,in , visited,labelToIndex)){
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



bool Optimizer::isUsedLaterGlobal(string& def, int index , vector<IRNode>& instructionList,vector<bool>& visited,	 map<string, int>& labelToIndex){
    
    //path ended
    if(index>=instructionList.size())
        return false;
    
    if(instructionList[index].opcode==Opcode::LABEL && instructionList[index].op3.substr(0,4).compare("FUNC")==0)
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
            return isUsedLaterGlobal(def, labelToIndex[instructionList[index].op3] ,instructionList, visited, labelToIndex);
        }
        //conditional jump
        else if(instructionList[index].opcode==Opcode::GT||
                instructionList[index].opcode==Opcode::GE||
                instructionList[index].opcode==Opcode::LT||
                instructionList[index].opcode==Opcode::LE||
                instructionList[index].opcode==Opcode::EQ||
                instructionList[index].opcode==Opcode::NE){
            //if used at any branch then return it is used
            return isUsedLaterGlobal(def, labelToIndex[instructionList[index].op3],instructionList, visited, labelToIndex) || isUsedLaterGlobal(def, index+1,instructionList, visited, labelToIndex);
        }
        else if(instructionList[index].opcode==Opcode::JSR){
            return isUsedLaterGlobal(def, labelToIndex[instructionList[index].op3],instructionList, visited, labelToIndex) || isUsedLaterGlobal(def, index+1,instructionList, visited, labelToIndex);
        }
        //not jump case
        else{
            return isUsedLaterGlobal(def, index+1,instructionList, visited, labelToIndex);
        }
        
    }
    
}




//ver 1 per function
void Optimizer::removeRedundantIRLocal(vector<IRNode>& in, string retOffset){
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
            
            if(in[i].opcode==Opcode::ADDI   ||
               in[i].opcode==Opcode::ADDF   ||
               in[i].opcode==Opcode::SUBI   ||
               in[i].opcode==Opcode::SUBF   ||
               in[i].opcode==Opcode::MULTI  ||
               in[i].opcode==Opcode::MULTF  ||
               in[i].opcode==Opcode::DIVI   ||
               in[i].opcode==Opcode::DIVF   ||
               in[i].opcode==Opcode::STOREF ||
               in[i].opcode==Opcode::STOREI )
            {
                
                string def=in[i].getDef();
                fill(visited.begin(),visited.end(), false);
                
                if( isGlobalVar(def) || def.compare(retOffset)==0)
                    continue;
                
                if(Optimizer::isUsedLaterLocal(def, i+1 ,in , visited,labelToIndex)){
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



bool Optimizer::isUsedLaterLocal(string& def, int index , vector<IRNode>& instructionList,vector<bool>& visited,	 map<string, int>& labelToIndex){
    
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
            return isUsedLaterLocal(def, labelToIndex[instructionList[index].op3] ,instructionList, visited, labelToIndex);
        }
        //conditional jump
        else if(instructionList[index].opcode==Opcode::GT||
                instructionList[index].opcode==Opcode::GE||
                instructionList[index].opcode==Opcode::LT||
                instructionList[index].opcode==Opcode::LE||
                instructionList[index].opcode==Opcode::EQ||
                instructionList[index].opcode==Opcode::NE){
            //if used at any branch then return it is used
            return isUsedLaterLocal(def, labelToIndex[instructionList[index].op3],instructionList, visited, labelToIndex) || isUsedLaterLocal(def, index+1,instructionList, visited, labelToIndex);
        }
        //not jump case
        else{
            return isUsedLaterLocal(def, index+1,instructionList, visited, labelToIndex);
        }
        
    }
    
}

void flipJumpOperands(IRNode* jumpIns){
    string tmpOp1=jumpIns->op1;
    jumpIns->op1=jumpIns->op2;
    jumpIns->op2=tmpOp1;
    
    switch(jumpIns->opcode){
        case Opcode::GT:
            jumpIns->opcode=Opcode::LT;
            break;
        case Opcode::LT:
            jumpIns->opcode=Opcode::GT;
            break;
        case Opcode::GE:
            jumpIns->opcode=Opcode::LE;
            break;
        case Opcode::LE:
            jumpIns->opcode=Opcode::GE;
            break;
        default:
            break;
    }
    
}

//per function
void Optimizer::optimizeInstructionSelection(vector<IRNode> & in){
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
        
        else if(in[i].opcode==Opcode::GT || in[i].opcode==Opcode::LT ||in[i].opcode==Opcode::GE ||in[i].opcode==Opcode::LE ||in[i].opcode==Opcode::EQ ||in[i].opcode==Opcode::NE ){
            if(findOperandType(in[i].op1)==OperandType::TEMP && findOperandType(in[i].op2)!=OperandType::TEMP)
                flipJumpOperands(&in[i]);
            
        }
        
        
        
    }
}

//to function
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
            if(instructionList[i].op3.size()>=4 &&instructionList[i].op3.substr(0,4).compare("FUNC")==0 )
                continue;
            
            instructionList.erase(instructionList.begin()+i);
            i--;
        }
    }
}

//to function
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

//per functions
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

//functions
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
void Optimizer::optimizeAllLocal(vector<IRNode>& in, string retOffset){
    int oldSize;
    do{
        oldSize=in.size();
        
        Optimizer:: removeRedundantIRLocal(in,retOffset);
        
        Optimizer::removeRepeatedExpression(in);
        Optimizer:: removeRedundantIRLocal(in,retOffset);
        
        
        Optimizer::reduceRedundantMoves(in);
        Optimizer::removeRedundantIRLocal(in,retOffset);
        
        
        Optimizer::evaluateConstantExpression(in);
        Optimizer::removeRedundantIRLocal(in,retOffset);
        
        
        Optimizer::optimizeInstructionSelection(in);
        Optimizer::removeRedundantIRLocal(in,retOffset);
        
        Optimizer::evaluateConstantJumps(in);
        Optimizer::removeUnReachableCode(in);
        Optimizer::removeRedundantJumps(in);
        Optimizer::removeRedundantLables(in);
        
    }while(oldSize!=in.size());
}
void Optimizer::optimizeAllGlobal(vector<IRNode>& in){
    int oldSize;
    do{
        oldSize=in.size();
        
        Optimizer:: removeRedundantIRGlobal(in);
        
    }while(oldSize!=in.size());
}
