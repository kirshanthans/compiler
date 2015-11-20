#ifndef BASICBLOCKS_H_
#define BASICBLOCKS_H_
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include"astnode.h"

class BasicBlock{
		public:
				int startId;
				int endId;
				vector<BasicBlock*> predecessor;
				vector<BasicBlock*> successor;
				bool isStart;
				BasicBlock(int startId_, int endId_, bool isStart_){
						startId     = startId_;
						endId       = endId_;
						isStart     = isStart_;
				}
};
vector<int> basicBlocksIndex(vector<IRNode> code);
string convertBlockSeparatedCodetoString(vector<IRNode> code, vector<int> bbEnd);
map<int, BasicBlock*> basicBlocks(vector<IRNode> code, vector<int> bbEnd);
int findJumpLabel(vector<IRNode> code, string label);
int findBasicBlockId(map<int, BasicBlock*> tab, int entry);
void printBasicBlock(map<int, BasicBlock*> tab);
#endif
