#include "astnode.h"

//*******************************************************************************
//get next if else labels
int ifelseLabel=0;
int ifEndLabel=0;

string generateElseLabel(){
		ostringstream newReg;
		newReg << "IF_ELSE_L" << ifelseLabel++;
		return newReg.str();
}
string generateIfEndLabel(){
		ostringstream newReg;
		newReg << "IF_END_L" << ifEndLabel++;
		return newReg.str();
}
//*******************************************************************************
//get next for labels
int forIncLabel=0;
int forOutLabel=0;
int forStartLabel=0;

string generateForOutLabel(){
		ostringstream newReg;
		newReg << "FOR_OUT_L" << forOutLabel++;
		return newReg.str();
}
string generateForIncLabel(){
		ostringstream newReg;
		newReg << "FOR_INC_L" << forIncLabel++;
		return newReg.str();
}

string generateForStartLabel(){
		ostringstream newReg;
		newReg << "FOR_START_L" << forStartLabel++;
		return newReg.str();
}
//*******************************************************************************
//get next temporary value
int tmpCount = 0;
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
		string code[] = {"addi", "addr", "subi", "subr", "muli", "mulr", "divi", "divr", "move", "sys readi", "sys readr", "sys writei", "sys writer", "sys writes", "inci", "deci", "cmpi", "cmpr", "push", "pop", "jsr", "ret", "link", "unlink", "label", "jmp", "jgt", "jlt", "jge", "jle", "jeq", "jne", "var", "str", "sys halt", "end"};
		return code[ins];
}
//convert list of IR ins into string
string convertCodetoString(vector<IRNode> code,string startS, string endS){
		ostringstream oss;
		oss << ";" << startS << endl;
		for(int i = 0;i < code.size(); ++i){
				oss << ";" << i << ">> " << code[i].toIRString() << endl;
		}
		oss << ";" << endS << endl;
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
string convertAssemblytoString(vector<AssemblyNode> code, bool isComment){
		ostringstream oss;
		for(vector<AssemblyNode>::iterator i = code.begin(); i != code.end(); ++i){
				if(isComment)
						oss << ";" << (*i).toString() << endl;
				else
						oss << (*i).toString() << endl;
		}
		return oss.str();
}
//find the operand type whether is a immediate or temporary or memory
OperandType findOperandType(string op){
		if(!op.substr(0, 1).compare("0")||!op.substr(0, 1).compare("1")||!op.substr(0, 1).compare("2")||!op.substr(0, 1).compare("3")||!op.substr(0, 1).compare("4")||!op.substr(0, 1).compare("5")||!op.substr(0, 1).compare("6")||!op.substr(0, 1).compare("7")||!op.substr(0, 1).compare("8")||!op.substr(0, 1).compare("9")||!op.substr(0, 1).compare(".")||!op.substr(0, 1).compare("-"))
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
				case Opcode::GT:
						{
								//if(findOperandType(this->op1)==OperandType::TEMP && findOperandType(this->op2)!=OperandType::TEMP)
								//		flipJumpOperands(this);

								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;

								if(findOperandType(this->op2)!=OperandType::TEMP){
										op2reg = getNextReg();
										assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, this->op2, op2reg));
								}
								assembly.push_back(AssemblyNode(this->isIntCompare?AssemblyIns::ASM_CMPI:AssemblyIns::ASM_CMPR ,op1reg, op2reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_JGT,this->op3,""));
								break;
						}
				case Opcode::GE:
						{
								//if(findOperandType(this->op1)==OperandType::TEMP && findOperandType(this->op2)!=OperandType::TEMP)
								//		flipJumpOperands(this);

								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;

								if(findOperandType(this->op2)!=OperandType::TEMP){
										op2reg=getNextReg();
										assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, this->op2, op2reg));
								}
								assembly.push_back(AssemblyNode(this->isIntCompare?AssemblyIns::ASM_CMPI:AssemblyIns::ASM_CMPR ,op1reg, op2reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_JGE,this->op3,""));
								break;
						}
				case Opcode::LT:
						{
								//if(findOperandType(this->op1)==OperandType::TEMP && findOperandType(this->op2)!=OperandType::TEMP)
								//		flipJumpOperands(this);

								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;

								if(findOperandType(this->op2)!=OperandType::TEMP){
										op2reg=getNextReg();
										assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, this->op2, op2reg));
								}
								assembly.push_back(AssemblyNode(this->isIntCompare?AssemblyIns::ASM_CMPI:AssemblyIns::ASM_CMPR ,op1reg, op2reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_JLT,this->op3,""));
								break;
						}
				case Opcode::LE:
						{
								//if(findOperandType(this->op1)==OperandType::TEMP && findOperandType(this->op2)!=OperandType::TEMP)
								//		flipJumpOperands(this);

								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;

								if(findOperandType(this->op2)!=OperandType::TEMP){
										op2reg=getNextReg();
										assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, this->op2, op2reg));
								}
								assembly.push_back(AssemblyNode(this->isIntCompare?AssemblyIns::ASM_CMPI:AssemblyIns::ASM_CMPR ,op1reg, op2reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_JLE,this->op3,""));
								break;
						}
				case Opcode::NE:
						{
								//if(findOperandType(this->op1)==OperandType::TEMP && findOperandType(this->op2)!=OperandType::TEMP)
								//		flipJumpOperands(this);

								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;

								if(findOperandType(this->op2)!=OperandType::TEMP){
										op2reg=getNextReg();
										assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, this->op2, op2reg));
								}
								assembly.push_back(AssemblyNode(this->isIntCompare?AssemblyIns::ASM_CMPI:AssemblyIns::ASM_CMPR ,op1reg, op2reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_JNE,this->op3,""));
								break;
						}
				case Opcode::EQ:
						{
								//if(findOperandType(this->op1)==OperandType::TEMP && findOperandType(this->op2)!=OperandType::TEMP)
								//		flipJumpOperands(this);

								string op1reg = (findOperandType(this->op1)==OperandType::TEMP)?getReg(this->op1):this->op1;
								string op2reg = (findOperandType(this->op2)==OperandType::TEMP)?getReg(this->op2):this->op2;

								if(findOperandType(this->op2)!=OperandType::TEMP){
										op2reg=getNextReg();
										assembly.push_back(AssemblyNode(AssemblyIns::ASM_MOVE, this->op2, op2reg));
								}
								assembly.push_back(AssemblyNode(this->isIntCompare?AssemblyIns::ASM_CMPI:AssemblyIns::ASM_CMPR ,op1reg, op2reg));
								assembly.push_back(AssemblyNode(AssemblyIns::ASM_JEQ,this->op3,""));
								break;
						}
				case Opcode::JUMP:
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_JMP,this->op3,""));
						break;
				case Opcode::LABEL:
						assembly.push_back(AssemblyNode(AssemblyIns::ASM_LABEL,this->op3,""));
						break;
				default:
						;
		}
		return assembly;
}

vector<IRNode> StatementList::genCode(){
		vector<IRNode> code;

		if(left != NULL){

				vector<IRNode> lcode = left->genCode();
				code.insert(code.end(), lcode.begin(), lcode.end());
		}
		if(right != NULL){

				vector<IRNode> rcode = right->genCode();
				code.insert(code.end(), rcode.begin(), rcode.end());

		}
		return code;
}

vector<IRNode> AssignStatement::genCode(){
		vector<IRNode> code;
		Identifier* leftId   = dynamic_cast<Identifier*>(this->left);

		vector<IRNode> lcode = left->genCode();
		vector<IRNode> rcode = right->genCode();

		code.insert(code.end(), rcode.begin(), rcode.end());

		if(this->valType==SymbolEntryType::INT)
				code.push_back(IRNode(Opcode::STOREI,this->right->getTempVal(),"",leftId->name));

		else if(this->valType==SymbolEntryType::FLOAT)
				code.push_back(IRNode(Opcode::STOREF,this->right->getTempVal(),"",leftId->name));


		return code;
}

vector<IRNode> Identifier::genCode(){
		vector<IRNode> code;
		return code;
}

vector<IRNode> Constant::genCode(){
		vector<IRNode> code;

		this->tempVal=getNextTemp();
		if(this->valType==SymbolEntryType::INT)
				code.push_back(IRNode(Opcode::STOREI, this->valAsString, "",this->tempVal ));
		else
				code.push_back(IRNode(Opcode::STOREF, this->valAsString, "",this->tempVal ));

		return code;
}

vector<IRNode> IOCommand::genCode(){
		vector<IRNode> code;

		for(int i=0; i<this->idList.size(); i++){
				Identifier * tmp=dynamic_cast<Identifier *>(idList[i]);

				if(isRead){
						if(tmp->valType==SymbolEntryType::INT)
								code.push_back(IRNode(READI,"","",tmp->name));
						else
								code.push_back(IRNode(READF,"","",tmp->name));

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

		return code;
}

vector<IRNode> ConditionExpression::genCode(){

		vector<IRNode> code;
		vector<IRNode> lcode = left->genCode();
		vector<IRNode> rcode = right->genCode();

		code.insert(code.end(), lcode.begin(), lcode.end());
		code.insert(code.end(), rcode.begin(), rcode.end());

		return code;
}

vector<IRNode> IfStatment::genCode(){

		vector<IRNode> code;
		vector<IRNode> conditionCode = this->cond->genCode();
		vector<IRNode> bodyCode      = this->stmt_list->genCode();
		vector<IRNode> elseCode      = this->else_stmt_list->genCode();

		code.insert(code.end(), conditionCode.begin(), conditionCode.end());

		string elseLabel = generateElseLabel();
		string endLabel  = generateIfEndLabel();

		ConditionExpression* condExp = dynamic_cast<ConditionExpression*>(this->cond);

		if(condExp->comparisonType == CompopType::COMP_EQ)
				code.push_back(IRNode(Opcode::NE, this->cond->left->getTempVal(), this->cond->right->getTempVal(), elseLabel, this->cond->valType == SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_NEQ)
				code.push_back(IRNode(Opcode::EQ, this->cond->left->getTempVal(), this->cond->right->getTempVal(), elseLabel, this->cond->valType == SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_LE)
				code.push_back(IRNode(Opcode::GE, this->cond->left->getTempVal(), this->cond->right->getTempVal(), elseLabel, this->cond->valType == SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_GR)
				code.push_back(IRNode(Opcode::LE, this->cond->left->getTempVal(), this->cond->right->getTempVal(), elseLabel, this->cond->valType == SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_LEQ)
				code.push_back(IRNode(Opcode::GT, this->cond->left->getTempVal(), this->cond->right->getTempVal(), elseLabel, this->cond->valType == SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_GEQ)
				code.push_back(IRNode(Opcode::LT, this->cond->left->getTempVal(), this->cond->right->getTempVal(), elseLabel, this->cond->valType == SymbolEntryType::INT));

		code.insert(code.end(), bodyCode.begin(), bodyCode.end());

		code.push_back(IRNode(Opcode::JUMP, "", "", endLabel));

		code.push_back(IRNode(Opcode::LABEL, "", "", elseLabel));

		code.insert(code.end(), elseCode.begin(), elseCode.end());

		code.push_back(IRNode(Opcode::LABEL, "", "", endLabel));

		return code;
}

vector<IRNode> ForStatment::genCode(){
		vector<IRNode> code;
		vector<IRNode> intialSatatmentCode   = this->initialStatment->genCode();
		vector<IRNode> incrementStatmentCode = this->incrementStatment->genCode();
		vector<IRNode> conditionCode         = this->cond->genCode();
		vector<IRNode> bodyCode              = this->body->genCode();

		string forStartLabel = generateForStartLabel();
		string forOutLabel   = generateForOutLabel();
		string forIncLabel   = generateForIncLabel();

		//add initial st
		code.insert(code.end(), intialSatatmentCode.begin(), intialSatatmentCode.end());
		//add start Label
		code.push_back(IRNode(Opcode::LABEL,"","",forStartLabel));
		//add condition expression code
		code.insert(code.end(), conditionCode.begin(), conditionCode.end());

		ConditionExpression* condExp = dynamic_cast<ConditionExpression*>(this->cond);
		//check if condition happen
		if(condExp->comparisonType == CompopType::COMP_EQ)
				code.push_back(IRNode(Opcode::NE, this->cond->left->getTempVal(), this->cond->right->getTempVal(), forOutLabel, this->cond->valType==SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_NEQ)
				code.push_back(IRNode(Opcode::EQ, this->cond->left->getTempVal(), this->cond->right->getTempVal(), forOutLabel, this->cond->valType==SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_LE)
				code.push_back(IRNode(Opcode::GE, this->cond->left->getTempVal(), this->cond->right->getTempVal(), forOutLabel, this->cond->valType==SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_GR)
				code.push_back(IRNode(Opcode::LE, this->cond->left->getTempVal(), this->cond->right->getTempVal(), forOutLabel, this->cond->valType==SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_LEQ)
				code.push_back(IRNode(Opcode::GT, this->cond->left->getTempVal(), this->cond->right->getTempVal(), forOutLabel, this->cond->valType==SymbolEntryType::INT));

		else if(condExp->comparisonType == CompopType::COMP_GEQ)
				code.push_back(IRNode(Opcode::LT, this->cond->left->getTempVal(), this->cond->right->getTempVal(), forOutLabel, this->cond->valType==SymbolEntryType::INT));


		for(int i=0; i < bodyCode.size(); i++){
				if(bodyCode[i].opcode == Opcode::JUMP && bodyCode[i].op3.compare("$continue") == 0)
						bodyCode[i].op3=forIncLabel;
				else if(bodyCode[i].opcode  ==  Opcode::JUMP && bodyCode[i].op3.compare("$break") == 0)
						bodyCode[i].op3=forOutLabel;
		}

		//add the body code
		code.insert(code.end(), bodyCode.begin(), bodyCode.end());
		//add the incLabel
		code.push_back(IRNode(Opcode::LABEL, "", "", forIncLabel));
		//add the inc code
		code.insert(code.end(), incrementStatmentCode.begin(), incrementStatmentCode.end());
		//add jump to start of code
		code.push_back(IRNode(Opcode::JUMP, "", "", forStartLabel));
		//add loop out label
		code.push_back(IRNode(Opcode::LABEL, "", "", forOutLabel));
		return code;
}

vector<IRNode> LoopControl::genCode(){
		vector<IRNode> code;
		if(isBreak)
				code.push_back(IRNode(Opcode::JUMP, "", "", "$break"));
		else
				code.push_back(IRNode(Opcode::JUMP, "", "", "$continue"));

		return code;
}
