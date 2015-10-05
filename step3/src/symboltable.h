#ifndef SCOPE_H_
#define SCOPE_H_

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>

using namespace std;

enum SymbolEntryType {SCOPE, INT, FLOAT, STRING};
enum ScopeType 		 {GLOBAL, FUNC, BLOCK};

class Scope;
class SymbolTableEntry;

class SymbolTableEntry{
		public :
				SymbolEntryType entryType;
				string id;
				string stringVal;
				Scope * scopeVal;
				SymbolTableEntry(SymbolEntryType entryType_, string id_, string stringVal_,	Scope * scope_ ):entryType(entryType_){
						id=id_;
						stringVal=stringVal_;
						scopeVal=scope_;
				}
};

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
				bool addSymbolEntry(string id_, SymbolEntryType entry_);
				bool addSymbolEntry(string id_ , string stringVal_);
				Scope* createChildScope(string functionName);
				Scope* createChildScope();
				Scope* getParentScope();
				string getScopeName();
				static void printShadowVar();
				void printSymbolTable();
				
		private:
				string scopeName;
				Scope* parentScope;
				map<string, int> identifiersLookUp;
				vector<Scope*> childScopesPool;
				vector<SymbolTableEntry> orderedEntries;
				ScopeType scopeType;
				static int blockCounter;
				static vector<string> shadowVar;
				
				bool idExistInParentScope(string id_);	
				bool idExistsInScope(string id_);

				

};

Scope* createGlobalScope();
#endif /* SCOPE_H_ */
