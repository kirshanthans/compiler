#include "astnode.h"
#define ENABLE_OPT1 1 //evaluating constant expressions
#define ENABLE_OPT2 1 //evaluating constant expressions

int tmpCount = 0;
//get next temporary value
string getNextTemp(){
		ostringstream newReg;
		newReg << "$" << "T" << tmpCount++;
		return newReg.str();
}
//get next register value for storing from mem to mem
string getNextReg(){
		ostringstream newReg;
		newReg << "r" << tmpCount++;
		return newReg.str();
}
//get the register name as string from temp var
string getReg(string temp){
		ostringstream newReg;
		newReg << "r" << temp.substr(2, temp.length());
		return newReg.str();
}
//get the opcode as string for a IR ins
string getIROpcodeString(Opcode opcode){
		string code[] =  {"ADDI", "ADDF", "SUBI", "SUBF", "MULTI", "MULTF", "DIVI", "DIVF", "STOREI", "STOREF", "GT", "GE", "LT", "LE", "NE", "EQ", "JUMP", "LABEL", "READI", "READF", "WRITEI", "WRITEF", "WRITES"};
		return code[opcode];
}
//get the opcode as a string for a assembly ins
string getAssemblyOpcodeString(AssemblyIns ins){
		string code[] = {"addi", "addr", "subi", "subr", "muli", "mulr", "divi", "divr", "move", "sys readi", "sys readr", "sys writei", "sys writer", "sys writes", "inci", "deci", "cmpi", "cmpr", "push", "pop", "jsr", "ret", "link", "unlink", "label", "jmp", "jqt", "jlt", "jge", "jle", "jeq", "jne", "var", "str", "sys halt", "end"};
		return code[ins];
}
//convert list of IR ins into string
string convertCodetoString(vector<IRNode> code){
		ostringstream oss;
		oss << ";IR code" << endl;
		for(vector<IRNode>::iterator i = code.begin(); i != code.end(); ++i){
				oss << ";" << (*i).toIRString() << endl;
		}
		oss << ";tiny code" << endl;
		return oss.str();
}
//assembly for declaration
vector<AssemblyNode> declAssembly(Scope* scope){
		vector<AssemblyNode> assembly;
		vector<SymbolTableEntry> entries = scope->getEntries();
		for(int i=0; i<entries.size(); i++){
				if(entries[i].entryType==SymbolEntryType::INT||entries[i].entryType==SymbolEntryType::FLOAT){
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_VAR, "", entries[i].id));
				}
				else if(entries[i].entryType==SymbolEntryType::STRING){
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_STR, entries[i].id, entries[i].stringVal));
				}
		}
		return assembly;

}
//convert list of IR ins to assembly ins 
vector<AssemblyNode> genAssembly(vector<IRNode> code){
		vector<AssemblyNode> assembly;
		vector<AssemblyNode> IR;
		for(vector<IRNode>::iterator i = code.begin(); i != code.end(); ++i){
				IR  = (*i).toAssembly();
				assembly.insert(assembly.end(), IR.begin(), IR.end());
		}
		assembly.push_back(AssemblyNode(AssemblyIns::ASM_HALT, "", ""));
		assembly.push_back(AssemblyNode(AssemblyIns::ASM_END, "", ""));
		return assembly;
}
//convert assembly code to string
string convertAssemblytoString(vector<AssemblyNode> code){
		ostringstream oss;
		for(vector<AssemblyNode>::iterator i = code.begin(); i != code.end(); ++i){
				oss << (*i).toString() << endl;
		}
		return oss.str();
}
//find the operand type whether is a immediate or temporary or memory
OperandType findOperandType(string op){
		if(!op.substr(0, 1).compare("0")||!op.substr(0, 1).compare("1")||!op.substr(0, 1).compare("2")||!op.substr(0, 1).compare("3")||!op.substr(0, 1).compare("4")||!op.substr(0, 1).compare("5")||!op.substr(0, 1).compare("6")||!op.substr(0, 1).compare("7")||!op.substr(0, 1).compare("8")||!op.substr(0, 1).compare("9")||!op.substr(0, 1).compare("."))
				return OperandType::IMM;
		else if(!op.substr(0,1).compare("$"))
				return OperandType::TEMP;
		else
				return OperandType::MEM;
}

//generate a string from assembly node
string AssemblyNode::toString(){
		string code;
		code.append(getAssemblyOpcodeString(opcode));

		if(!op1.empty()){
				code.append(" ");
				code.append(op1);
		}
		if(!op2.empty()){
				code.append(" ");
				code.append(op2);
		}
		return code;
}
//get the defined temp in a IR node
string IRNode::getDef(){
		switch (this->opcode) {
				case Opcode::ADDI:
				case Opcode::ADDF:
				case Opcode::SUBI:
				case Opcode::SUBF:
				case Opcode::MULTI:
				case Opcode::MULTF:
				case Opcode::DIVI:
				case Opcode::DIVF:
				case Opcode::STOREI:
				case Opcode::STOREF:
				case Opcode::READI:
				case Opcode::READF:
						return this->op3;
						break;
				default:
						return "";
						break;
		}
}
//check whether a previous definition is used in this IR node
bool IRNode::isDef(string def){
		if(def.compare(getDef())==0)
				return true;
		else return false;

}

bool IRNode::isAUse(string reg){
		switch (opcode) {
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
						if(this->op1.compare(reg) == 0||this->op2.compare(reg) == 0)
								return true;
						else
								return false;

						break;
				case Opcode::STOREI:
				case Opcode::STOREF:
						if(this->op1.compare(reg) == 0)
								return true;
						else
								return false;
						break;
				case Opcode::WRITEF:
				case Opcode::WRITEI:
						if(this->op3.compare(reg) == 0)
								return true;
						else
								return false;

						break;
				default:
						return false;
		}
}
//get IR node to string
string IRNode::toIRString(){
		string code;
		code.append(getIROpcodeString(opcode));

		if(!op1.empty()){
				code.append(" ");
				code.append(op1);
		}
		if(!op2.empty()){
				code.append(" ");
				code.append(op2);
		}
		if(!op3.empty()){
				code.append(" ");
				code.append(op3);
		}
		return code;
}
//assembly node generation
vector<AssemblyNode> IRNode::toAssembly(){
		vector<AssemblyNode> assembly;
		switch (this->opcode){
				case Opcode::ADDI:
						{
								string reg = getReg(this->op3);
								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, op1reg, reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_ADDI, op2reg, reg));
						}
						break;
                case Opcode::ADDF:
						{
								string reg = getReg(this->op3);
								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, op1reg, reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_ADDR, op2reg, reg));
						}
						break;
                case Opcode::SUBI:
						{
								string reg = getReg(this->op3);
								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, op1reg, reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_SUBI, op2reg, reg));
						}
						break;
                case Opcode::SUBF:
						{
								string reg = getReg(this->op3);
								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, op1reg, reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_SUBR, op2reg, reg));
						}
						break;
				case Opcode::MULTI:
						{
								string reg = getReg(this->op3);
								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, op1reg, reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MULTI, op2reg, reg));
						}
						break;
				case Opcode::MULTF:
						{
								string reg = getReg(this->op3);
								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, op1reg, reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MULTR, op2reg, reg));
						}
						break;
                case Opcode::DIVI:
						{
								string reg = getReg(this->op3);
								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, op1reg, reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_DIVI, op2reg, reg));
						}
						break;
                case Opcode::DIVF:
						{
								string reg = getReg(this->op3);
								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, op1reg, reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_DIVR, op2reg, reg));
						}
						break;
				case Opcode::STOREI:
				case Opcode::STOREF:
						{
								if(findOperandType(this->op1)==OperandType::MEM && findOperandType(this->op3)==OperandType::MEM){
										string reg = getNextReg();
										assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, this->op1, reg));
										assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, reg, this->op3));
								}
								else{
										string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
										string op3reg = (findOperandType(this->op3)==OperandType::TEMP)?getReg(this->op3):this->op3;
										assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, op1reg, op3reg));
								}
						}
						break;
				case Opcode::READI:
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_READI, "", this->op3));
						break;
				case Opcode::READF:
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_READR, "", this->op3));
						break;
				case Opcode::WRITEI:
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_WRITEI, "", this->op3));
						break;
				case Opcode::WRITEF:
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_WRITER, "", this->op3));
						break;
				case Opcode::WRITES:
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_WRITES, "", this->op3));
						break;
				default:
						;
		}
		return assembly;
}

vector<IRNode> StatementList::genCode(){
		vector<IRNode> code;

		vector<IRNode> lcode = left->genCode();
		code.insert(code.end(), lcode.begin(), lcode.end());

		if(right != NULL){
				vector<IRNode> rcode = right->genCode();
				code.insert(code.end(), rcode.begin(), rcode.end());

		}
		return code;
}

vector<IRNode> AssignStatement::genCode(){
		vector<IRNode> code;
		Identifier * leftId=dynamic_cast<Identifier *>(this->left);
		vector<IRNode> lcode = left->genCode();
		vector<IRNode> rcode = right->genCode();

		if(ENABLE_OPT1==1 && right->hasImmediateValue()){
				if(this->valType==SymbolEntryType::INT)
						code.push_back(IRNode(Opcode::STOREI,right->getImmediateValueAsString(),"",leftId->name));

				else if(this->valType==SymbolEntryType::FLOAT)
						code.push_back(IRNode(Opcode::STOREF,right->getImmediateValueAsString(),"",leftId->name));
				
				leftId->scope->updateIdValue(leftId->name,right->getImmediateValue());
		}
		else{
				code.insert(code.end(), rcode.begin(), rcode.end());
				
				if(this->valType==SymbolEntryType::INT)
						code.push_back(IRNode(Opcode::STOREI,this->right->getTempVal(),"",leftId->name));

				else if(this->valType==SymbolEntryType::FLOAT)
						code.push_back(IRNode(Opcode::STOREF,this->right->getTempVal(),"",leftId->name));

				leftId->scope->cancelIdValue(leftId->name);

				if(ENABLE_OPT2==1){
						leftId->scope->updateIdTempValue(leftId->name,right->getTempVal());
				}

		}

		return code;
}

vector<IRNode> Identifier::genCode(){
		vector<IRNode> code;
		return code;
}

vector<IRNode> Constant::genCode(){
		vector<IRNode> code;

		if(ENABLE_OPT1==1){
				//no need to move the value into register
				if(valType==SymbolEntryType::FLOAT)
						immediateVal.floatVal=stof(valAsString);
				else
						immediateVal.intval=stoi(valAsString);
		}
		else {
				this->tempVal=getNextTemp();
				if(this->valType==SymbolEntryType::INT)
						code.push_back(IRNode(Opcode::STOREI, this->valAsString, "",this->tempVal ));
				else
						code.push_back(IRNode(Opcode::STOREF, this->valAsString, "",this->tempVal ));
		}
		return code;
}

vector<IRNode> IOCommand::genCode(){
		vector<IRNode> code;
		for(int i=0; i<this->idList.size(); i++){
				Identifier * tmp=dynamic_cast<Identifier*>(idList[i]);

				if(isRead){
						if(tmp->valType==SymbolEntryType::INT)
								code.push_back(IRNode(READI,"","",tmp->name));
						else if(tmp->valType==SymbolEntryType::FLOAT)
								code.push_back(IRNode(READF,"","",tmp->name));
						
						tmp->scope->cancelIdValue(tmp->name);
						tmp->scope->cancelIdTempValue(tmp->name);
				}
				else{
						if(tmp->valType==SymbolEntryType::INT)
								code.push_back(IRNode(WRITEI,"","",tmp->name));
						else if(tmp->valType==SymbolEntryType::FLOAT)
								code.push_back(IRNode(WRITEF,"","",tmp->name));
						else
								code.push_back(IRNode(WRITES,"","",tmp->name));
				}
		}

		return code;
}

vector<IRNode> Binop::genCode(){
		vector<IRNode> code;
		vector<IRNode> lcode = left->genCode();
		vector<IRNode> rcode = right->genCode();

		if(ENABLE_OPT1==1 &&(left->hasImmediateValue() || right->hasImmediateValue())  ){
				//if both right and left side are known
				if(right->hasImmediateValue()&& left->hasImmediateValue()){
						this->valueKnown=true;

						switch (this->operationType) {
								case BinopType::ADD:
										if(valType==FLOAT)
												this->setImmediateValue(right->getImmediateValue().floatVal + left->getImmediateValue().floatVal);
										else
												this->setImmediateValue(right->getImmediateValue().intval+left->getImmediateValue().intval);

										break;
								case BinopType::SUB:
										if(valType==FLOAT)
												this->setImmediateValue(left->getImmediateValue().floatVal-right->getImmediateValue().floatVal);
										else
												this->setImmediateValue(left->getImmediateValue().intval-right->getImmediateValue().intval);
										break;
								case BinopType::MUL:
										if(valType==FLOAT){
												this->setImmediateValue(right->getImmediateValue().floatVal*left->getImmediateValue().floatVal);
										}else
												this->setImmediateValue(right->getImmediateValue().intval*left->getImmediateValue().intval);
										break;
								case BinopType::DIV:
										if(valType==FLOAT)
												this->setImmediateValue(left->getImmediateValue().floatVal/right->getImmediateValue().floatVal);
										else
												this->setImmediateValue(left->getImmediateValue().intval/right->getImmediateValue().intval);

										break;
								default:
										break;
						}
				}
				else{
						string leftOp,rightOp,res;
						if(!right->hasImmediateValue()){
								code.insert(code.end(), rcode.begin(), rcode.end());
								rightOp=right->getTempVal();
						}
						else{
								rightOp=right->getImmediateValueAsString();
						}

						if(!left->hasImmediateValue()){
								code.insert(code.end(), lcode.begin(), lcode.end());
								leftOp=left->getTempVal();
						}
						else{
								leftOp=left->getImmediateValueAsString();
						}
						res=getNextTemp();
						this->tempVal=res;
						switch (this->operationType) {
								case BinopType::ADD:
										if(this->valType==SymbolEntryType::INT)
												code.push_back(IRNode(ADDI,leftOp,rightOp,res));
										else
												code.push_back (IRNode(ADDF,leftOp,rightOp,res));
										break;
								case BinopType::SUB:
										if(this->valType==SymbolEntryType::INT)
												code.push_back(IRNode(SUBI,leftOp,rightOp,res));
										else
												code.push_back(IRNode(SUBF,leftOp,rightOp,res));
										break;
								case BinopType::MUL:
										if(this->valType==SymbolEntryType::INT)
												code.push_back(IRNode(MULTI,leftOp,rightOp,res));
										else
												code.push_back(IRNode(MULTF,leftOp,rightOp,res));
										break;
								case BinopType::DIV:
										if(this->valType==SymbolEntryType::INT)
												code.push_back(IRNode(DIVI,leftOp,rightOp,res));
										else
												code.push_back(IRNode(DIVF,leftOp,rightOp,res));
										break;
								default:
										break;
						}
				}
		}
		else{//no optimizations
				string leftOp,rightOp,res;

				code.insert(code.end(), lcode.begin(), lcode.end());
				leftOp=left->getTempVal();

				code.insert(code.end(), rcode.begin(), rcode.end());
				rightOp=right->getTempVal();

				res=getNextTemp();
				this->tempVal=res;
				switch (this->operationType) {
						case BinopType::ADD:
								if(this->valType==SymbolEntryType::INT)
										code.push_back(IRNode(ADDI,leftOp,rightOp,res));
								else
										code.push_back (IRNode(ADDF,leftOp,rightOp,res));
								break;
						case BinopType::SUB:
								if(this->valType==SymbolEntryType::INT)
										code.push_back(IRNode(SUBI,leftOp,rightOp,res));
								else
										code.push_back(IRNode(SUBF,leftOp,rightOp,res));
								break;
						case BinopType::MUL:
								if(this->valType==SymbolEntryType::INT)
										code.push_back(IRNode(MULTI,leftOp,rightOp,res));
								else
										code.push_back(IRNode(MULTF,leftOp,rightOp,res));
								break;
						case BinopType::DIV:
								if(this->valType==SymbolEntryType::INT)
										code.push_back(IRNode(DIVI,leftOp,rightOp,res));
								else
										code.push_back(IRNode(DIVF,leftOp,rightOp,res));
								break;
						default:
								break;
				}
		}
		return code;
}


bool Constant::hasImmediateValue(){
		return true;
}

bool Identifier::hasImmediateValue(){
		return scope->getIdInfo(this->name).valueKnown;
}

bool Binop::hasImmediateValue(){
		return this->valueKnown;
}

ValueHolder Identifier::getImmediateValue(){
		if(!scope->getIdInfo(this->name).valueKnown)
				;
		return scope->getIdInfo(this->name).val;
}

string Identifier::getTempVal(){
		if(ENABLE_OPT2==1)
				return scope->getIdInfo(this->name).tempVal;
		else
				return name;

}
