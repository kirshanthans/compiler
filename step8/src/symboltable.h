#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <stack>

using namespace std;

enum SymbolEntryType {SCOPE=1, INT=2, FLOAT=3, STRING=4, VOID=5};
enum ScopeType 		 {GLOBAL, FUNC, BLOCK};
union ValueHolder    {int intval; float floatVal;};

class Scope;
class SymbolTableEntry;

class Scope{
		public:
				Scope(ScopeType scopeType_, string name_,Scope* parentScope_){
						scopeType=scopeType_;
						scopeName=name_;
						parentScope=parentScope_;
						childScopesPool.clear();
						orderedEntries.clear();
				}
				~Scope(){
						for(int i = 0; i < childScopesPool.size(); i++)
								delete childScopesPool[i];

						if(this->scopeType == GLOBAL)
								delete this;
				}
				bool addSymbolEntry(string id_, SymbolEntryType entry_, bool isParameter);
				bool addSymbolEntry(string id_ , string stringVal_, bool isParameter);
				Scope* createChildScope(string functionName, SymbolEntryType retValType);
				Scope* createChildScope();
				Scope* getParentScope();
				ScopeType getScopeType();
				string getScopeName();
				void printSymbolTable();
				SymbolEntryType getIdType(string id_);
				SymbolTableEntry getIdInfo(string id_);
				vector<SymbolTableEntry> getEntries();
				bool isLocal(string id_);
				string getVariableFullName(string id_);
				static Scope* getFunctionScope(string functionName);
				static Scope* globalScope;
				static Scope* createGlobalScope();
				static void evaluateAllLocalOffsets();
				static void evaluateLocalOffsets(Scope* functionScope);
				static void evaluateBlockOffset(int& k, Scope* blockScope);
				static Scope* getParentFunction(Scope* currentScope);

				int retOffset;//only used with function
				SymbolEntryType retValType;//b
				int localBlockSize;
				vector<SymbolTableEntry> orderedEntries;

		private:
				string scopeName;
				Scope* parentScope;
				map<string, int> identifiersLookUp;
				vector<Scope*> childScopesPool;
				ScopeType scopeType;
				static int blockCounter;

				bool idExistInParentScope(string id_);	
				bool idExistsInScope(string id_);

};

class SymbolTableEntry {
		public :
				SymbolEntryType entryType;
				string id;
				string stringVal;
				Scope* scopeVal;
				int localOffset; //only used with local variable
				bool isLocal;
				bool isParameter;
				SymbolTableEntry(SymbolEntryType entryType_, string id_, string stringVal_,	Scope* scope_ ,bool isParameter_ ):entryType(entryType_){
						id=id_;
						stringVal=stringVal_;
						scopeVal=scope_;
						isLocal=(scope_-> getScopeType() != ScopeType::GLOBAL);
						isParameter=isParameter_;
				}

};
#endif 
/* SYMBOLTABLE_H_*/
