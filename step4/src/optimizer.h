#ifndef OPTIMIZER_H_
#define OPTIMIZER_H_

#include<map>
#include"astnode.h"

using namespace std;

namespace Optimizer{
		void updateDefName(vector<IRNode>& instructionList, int starting_index, string newDefName, string oldDefName);
		void removeRedundantExpression(vector<IRNode>& instructionList);
		void removeRedundantIR(vector<IRNode>& in);
		void optimizeInstructionSelection(vector<IRNode>& in);

};

#endif
