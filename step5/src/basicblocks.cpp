#include "basicblocks.h"
using namespace std;
vector<int> basicBlocksIndex(vector<IRNode> code){
		vector<int> bb;
		for(int i=0; i<code.size(); i++){
				IRNode ir = code[i];
				if(ir.opcode == Opcode::LABEL && i != 0){
						if(bb.size() > 0){
						   if(bb.back() != i-1){
								   bb.push_back(i-1);
						   }
						}
						else{
								   bb.push_back(i-1);
						}
				}
				else if(ir.opcode == Opcode::JUMP||ir.opcode == Opcode::GT||ir.opcode == Opcode::GE||ir.opcode == Opcode::LT||ir.opcode == Opcode::LE||ir.opcode == Opcode::NE||ir.opcode == Opcode::EQ){
						bb.push_back(i);
				}
		}
		if((bb.size() > 0 && bb.back() != code.size()-1) || bb.size() == 0) bb.push_back(code.size()-1);
		return bb;
}
string convertBlockSeparatedCodetoString(vector<IRNode> code, vector<int> bb){
	ostringstream oss;
	int j = 0;
	for(int i = 0; i < code.size(); ++i){
			oss << ";" << code[i].toIRString() << endl;
			if(bb[j] == i){
					oss << ";*****End of Basic Block " << j << " *****" << endl;
					j++;
			}
	}
	oss << ";tiny code" << endl;
	return oss.str();
}
map<int, BasicBlock*> basicBlocks(vector<IRNode> code, vector<int> bbEnd){
		map<int, BasicBlock*> tab;
		vector<int> bbStart;
		bbStart.push_back(0);
		for(int i=0; i<bbEnd.size()-1; ++i){
				if(bbEnd[i] > 0)
						bbStart.push_back(bbEnd[i]+1);
		}
		for(int i=0; i<bbEnd.size(); ++i){
				if(i == 0){
						tab[i] = new BasicBlock(bbStart[i], bbEnd[i], true);
				}
				else{
						tab[i] = new BasicBlock(bbStart[i], bbEnd[i], false);
				}
		}
		int s,e, block;
		Opcode op;
		for(int i=0; i<tab.size(); ++i){
				//cout << "debug" << endl;
				s = tab[i]->startId;
				e = tab[i]->endId;
				op = code[e].opcode;
				if(op == Opcode::JUMP){
						block = findBasicBlockId(tab, findJumpLabel(code, code[e].op3));
						tab[i]->successor.push_back(tab[block]);
						tab[block]->predecessor.push_back(tab[i]);
				}
				else if(op == Opcode::GT||op == Opcode::GE||op == Opcode::LT||op == Opcode::LE||op == Opcode::NE||op == Opcode::EQ){
						block = findBasicBlockId(tab, findJumpLabel(code, code[e].op3));
						tab[i]->successor.push_back(tab[block]);
						tab[i]->successor.push_back(tab[i+1]);
						tab[block]->predecessor.push_back(tab[i]);
						tab[i+1]->predecessor.push_back(tab[i]);
				}
				else{
						if(i != tab.size()-1){
								tab[i]->successor.push_back(tab[i+1]);
								tab[i+1]->predecessor.push_back(tab[i]);
						}
				}
		}
		return tab;
}
int findJumpLabel(vector<IRNode> code, string label){
		for(int i=0; i<code.size(); ++i){
				if(code[i].opcode == Opcode::LABEL && code[i].op3.compare(label) == 0)
						return i;
		}
		return -1;
}
int findBasicBlockId(map<int, BasicBlock*> tab, int entry){
		for(int i=0; i<tab.size(); ++i){
				if(tab[i]->startId == entry)
						return i;
		}
		return -1;
}
void printBasicBlock(map<int, BasicBlock*> tab){
		int num = tab.size();
		for(int i=0; i<num; ++i){
				cout << ";Basic Block " << i << " Start at Ins: " << tab[i]->startId << " Ends at Ins: " << tab[i]->endId << endl;
			    cout << ";Number of Successors: " << tab[i]->successor.size() << endl;
				cout<< ";Successors: ";
				for(int j=0; j<tab[i]->successor.size(); ++j)
						cout << tab[i]->successor[j]->startId << " ";
				cout << endl;
			    cout << ";Number of Predecessors: " << tab[i]->predecessor.size() << endl;
				cout<< ";Predecessors: ";
				for(int j=0; j<tab[i]->predecessor.size(); ++j)
						 cout << tab[i]->predecessor[j]->startId << " ";
				cout << endl;
		}
}

