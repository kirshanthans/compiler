#include "symboltable.h"
#include "register.h"
int Scope::blockCounter=1;
Scope* Scope::globalScope=NULL;

bool Scope::addSymbolEntry(string id_, SymbolEntryType entry_, bool isParameter){
		//check that entry not already exists
		if(idExistsInScope(id_)){
				cout << "DECLARATION ERROR " <<  id_ << endl;
				return false;
		}
		if(idExistInParentScope(id_))
				cout << "SHADOW WARNING " << id_ << endl;


		orderedEntries.push_back(SymbolTableEntry(entry_, id_, "", this, isParameter));
		identifiersLookUp[id_] = orderedEntries.size()-1;
		return true;
}

bool Scope::addSymbolEntry(string id_ , string stringVal_, bool isParameter){
		//check that entry not already exists
		if(idExistsInScope(id_)){
				cout << "DECLARATION ERROR " <<  id_ << endl;
				return false;
		}
		if(idExistInParentScope(id_))
				cout << "SHADOW WARNING " << id_ << endl;


		orderedEntries.push_back(SymbolTableEntry(STRING, id_, stringVal_, this, isParameter));
		identifiersLookUp[id_] = orderedEntries.size()-1;
		return true;
}

Scope* Scope::createChildScope(string functionName, SymbolEntryType retValType_){
		//add the new scope to the pool of child scopes
		Scope* newScope = new Scope(FUNC, functionName, this);

		newScope->retValType=retValType_;
		childScopesPool.push_back(newScope);
		//add the new scope in the ordered list because we need to track correct orders
		string dummyId = "FUNC_"+functionName;
		identifiersLookUp[dummyId] = orderedEntries.size();
		orderedEntries.push_back(SymbolTableEntry(SCOPE, dummyId, "", newScope, false));

		return newScope;
}

Scope* Scope::createChildScope(){
		//add the new scope to the pool of child scopes
		stringstream ss;
		ss << "BLOCK " << blockCounter++;

		string blockName = ss.str();
		Scope * newScope = new Scope(BLOCK, blockName, this);

		childScopesPool.push_back(newScope);
		//add the new scope in the ordered list because we need to track correct orders
		string dummyId = blockName;
		orderedEntries.push_back(SymbolTableEntry(SCOPE, dummyId, "", newScope, false));

		return newScope;
}

Scope* Scope::getParentScope(){
		return this->parentScope;
}

ScopeType Scope::getScopeType(){
		return this->scopeType;
}

string Scope::getScopeName(){
		return this->scopeName;
}

void Scope::printSymbolTable(){
		cout << "Symbol table " << scopeName << endl;

		for(int i=0; i<orderedEntries.size(); i++){

				if(orderedEntries[i].entryType==SCOPE){
						cout << endl;
						orderedEntries[i].scopeVal->printSymbolTable();
				}

				else if(orderedEntries[i].entryType == INT)
						cout << "name " << orderedEntries[i].id << " type INT" << endl;

				else if(orderedEntries[i].entryType == FLOAT)
						cout << "name " << orderedEntries[i].id << " type FLOAT" << endl;

				else if(orderedEntries[i].entryType == STRING)
						cout << "name " << orderedEntries[i].id << " type STRING value " << orderedEntries[i].stringVal << endl;
		}
}

bool Scope::idExistInParentScope(string id_){
		if(this->parentScope == NULL)
				return false;
		return this->parentScope->idExistInParentScope(id_);
}

bool Scope::idExistsInScope(string id_){
		if(identifiersLookUp.find(id_) == identifiersLookUp.end())
				return false;
		else
				return true;
}

Scope*  Scope::createGlobalScope(){
		Scope* ret  = new Scope(GLOBAL, "GLOBAL", NULL);
		Scope::globalScope = ret;
		return ret;
}

SymbolEntryType Scope::getIdType(string id_){
		if(idExistsInScope(id_))
				return orderedEntries[this->identifiersLookUp[id_]].entryType;
		else
				return parentScope->getIdType(id_);

}

SymbolTableEntry Scope::getIdInfo(string id_){
		if(idExistsInScope(id_))
				return orderedEntries[this->identifiersLookUp[id_]];
		else
				return this->parentScope->getIdInfo(id_);
}

vector<SymbolTableEntry> Scope::getEntries(){
		return orderedEntries;
}

string Scope::getVariableFullName(string id_){

		if(this->idExistsInScope(id_)){
				return getScopeName()+"_"+id_;
		}
		else{
				return getParentScope()->getVariableFullName(id_);
		}

}

Scope* Scope::getFunctionScope(string functionName){
		string scopeName="FUNC_"+functionName;
		return  Scope::globalScope->orderedEntries[globalScope->identifiersLookUp[scopeName]].scopeVal;

}

void Scope::evaluateAllLocalOffsets(){
		for(int i=0; i<Scope::globalScope->orderedEntries.size(); i++){
				if(Scope::globalScope->orderedEntries[i].scopeVal->scopeType==ScopeType::FUNC){
						Scope::evaluateLocalOffsets(Scope::globalScope->orderedEntries[i].scopeVal);

				}
		}
}

void Scope::evaluateLocalOffsets(Scope* functionScope){
		//compute parameters
		int maxi=functionScope->orderedEntries.size();
		int i=0;

		while(i<maxi &&functionScope->orderedEntries[i].isParameter==true ) {
				i++;
		}
		int offset = i+1;

		for(int j=0; j<i; j++){
				functionScope->orderedEntries[j].localOffset = offset--;
				//cout << "debug offset " << functionScope->orderedEntries[j].localOffset << endl;
		}

		functionScope->retOffset = i+2+NUMREG;

		//compute local variable not nested
		int k=1;
		while(  i<maxi && functionScope->orderedEntries[i].entryType!= SymbolEntryType::SCOPE){
				functionScope->orderedEntries[i].localOffset=-k;
				k++;
				i++;
		}

		//compute nested local variables
		while (i<maxi){
				evaluateBlockOffset(k,functionScope->orderedEntries[i].scopeVal);
				i++;
		}
		functionScope->localBlockSize=k;

}

void Scope::evaluateBlockOffset(int& k, Scope* blockScope){
		for(int i=0; i<blockScope->orderedEntries.size(); i++){
				if(blockScope->orderedEntries[i].entryType==SymbolEntryType::SCOPE)
						evaluateBlockOffset(k, blockScope->orderedEntries[i].scopeVal);
				else{
						blockScope->orderedEntries[i].localOffset = -k;
						k++;
				}
		}
}

Scope* Scope::getParentFunction(Scope* currentScope){
		if(currentScope->scopeType==ScopeType::FUNC)
				return currentScope;
		else
				return getParentFunction(currentScope->getParentScope());
}
