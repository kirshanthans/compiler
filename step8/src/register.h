#ifndef REGISTER_H_
#define REGISTER_H_
#include "astnode.h"
#include "basicblocks.h"
#define NUMREG 4

class Register;

using namespace std;

class RegisterAllocator{
		private:
				string regFile[NUMREG];
				bool   isDirty[NUMREG];
				bool   isEmpty[NUMREG];
				int ensure(vector<AssemblyNode>& assembly,IRNode ins, string op);
				bool existsInRegFile(string op);
				void free(vector<AssemblyNode>& assembly, IRNode ins, int i);
				int allocate(vector<AssemblyNode>& assembly, IRNode ins, string op);
				int choose(const IRNode  &  ins,string op );
				void resetAndWriteBackEndOfBB(vector<AssemblyNode>& assembly);
				void writeBackGlobalVariables(vector<AssemblyNode>& assembly);
				void printRegTable();
		public:
				static set<string> newMemLocations;
				RegisterAllocator(){
						for(int i = 0; i < NUMREG; ++i){
								this->regFile[i] = "";
								this->isDirty[i] = false;
								this->isEmpty[i] = true;
						}
				};
				~RegisterAllocator(){};
				vector<AssemblyNode> genAllocate(vector<IRNode> & ins);
				static string regStr(int i); 
				static string getOperandMemLocation(string op);
};
#endif
