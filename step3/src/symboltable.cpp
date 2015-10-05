#include "symboltable.h"

int Scope::blockCounter=1;
vector<string> Scope::shadowVar;

bool Scope::addSymbolEntry(string id_, SymbolEntryType entry_){
		//check that entry not already exists
		if(idExistsInScope(id_)){
				cout << "DECLARATION ERROR " <<  id_ << endl;
				return false;
		}
		if(idExistInParentScope(id_))
				if(find(shadowVar.begin(), shadowVar.end(), id_) == shadowVar.end())
						shadowVar.push_back(id_);

		orderedEntries.push_back(SymbolTableEntry(entry_, id_ , "", NULL));
		identifiersLookUp[id_] = orderedEntries.size()-1;

		return true;
}

bool Scope::addSymbolEntry(string id_ , string stringVal_){
		//check that entry not already exists
		if(idExistsInScope(id_)){
				cout << "DECLARATION ERROR " <<  id_ << endl;
				return false;
		}
		if(idExistInParentScope(id_))
				if(find(shadowVar.begin(), shadowVar.end(), id_) == shadowVar.end())
						shadowVar.push_back(id_);

		orderedEntries.push_back(SymbolTableEntry(STRING, id_, stringVal_, NULL));
		identifiersLookUp[id_] = orderedEntries.size()-1;
		return true;
}

Scope* Scope::createChildScope(string functionName){
		//add the new scope to the pool of child scopes
		Scope * newScope = new Scope(FUNC, functionName, this );

		childScopesPool.push_back(newScope);
		//add the new scope in the ordered list because we need to track correct orders
		string dummyId = "$"+functionName+"$";
		orderedEntries.push_back(SymbolTableEntry(SCOPE, dummyId, "", newScope));

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
		string dummyId = "$"+blockName+"$";
		orderedEntries.push_back(SymbolTableEntry(SCOPE, dummyId, "", newScope));

		return newScope;
}

Scope* Scope::getParentScope(){
		return parentScope;
}

string Scope::getScopeName(){
		return scopeName;
}

void Scope::printShadowVar(){
		if(shadowVar.size()){
				for(vector<string>::iterator i = shadowVar.begin(); i != shadowVar.end(); ++i)
						cout << "SHADOW WARNING " << *i << endl;
		}
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
						cout << "name " << orderedEntries[i].id << " type STRING value " << &orderedEntries[i].stringVal[0] << endl;
		}
}

bool Scope::idExistInParentScope(string id_){
		if(parentScope == NULL)
				return false;
		else{
				if(parentScope->idExistsInScope(id_))
						return true;
				else
						return parentScope->idExistInParentScope(id_);
		}
}

bool Scope::idExistsInScope(string id_){
		if(identifiersLookUp.find(id_) == identifiersLookUp.end())
				return false;
		else
				return true;
}

Scope* createGlobalScope(){
		return  new Scope(GLOBAL, "GLOBAL", NULL);
}
