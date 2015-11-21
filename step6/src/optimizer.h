#ifndef OPTIMIZER_H_
#define OPTIMIZER_H_
#include<map>
#include<set>
#include"astnode.h"
using namespace std;
namespace Optimizer{
		void updateDefName(vector<IRNode>& instructionList, int starting_index, string newDefName, string oldDefName);
        void updateDefNameConstExp(vector<IRNode>& instructionList, int starting_index, string newDefName, string oldDefName);

    void reduceRedundantMoves(vector<IRNode>& instructionList);
		void removeRepeatedExpression(vector<IRNode>& instructionList);
		void evaluateConstantExpression(vector<IRNode>& instructionList);
		void removeRedundantIRLocal(vector<IRNode>& in, string retOffset);
        void removeRedundantIRGlobal(vector<IRNode>& in);

		bool isUsedLaterLocal(string & def, int index , vector<IRNode>  & instructionList,vector<bool > &visited ,	 map<string, int>  & labelToIndex);
        bool isUsedLaterGlobal(string & def, int index , vector<IRNode>  & instructionList,vector<bool > &visited ,	 map<string, int>  & labelToIndex);

        void optimizeInstructionSelection(vector<IRNode>& in);
		void removeRedundantLables(vector<IRNode>& instructionList);
		void removeRedundantJumps(vector<IRNode>& instructionList);
		void removeUnReachableCode(vector<IRNode>& instructionList);
		void evaluateConstantJumps(vector<IRNode>& instructionList);
		void optimizeAllLocal(vector<IRNode>& in, string retOffset);
        void optimizeAllGlobal(vector<IRNode>& in);

};
#endif
