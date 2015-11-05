#include "symboltable.h"

int Scope::blockCounter=1;

bool Scope::addSymbolEntry(string id_, SymbolEntryType entry_){
		//check that entry not already exists
		if(idExistsInScope(id_)){
				cout << "DECLARATION ERROR " <<  id_ << endl;
				return false;
		}
		if(idExistInParentScope(id_))
				cout << "SHADOW WARNING " << id_ << endl;

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
				cout << "SHADOW WARNING " << id_ << endl;

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
				return parentScope->getIdInfo(id_);
}
vector<SymbolTableEntry> Scope::getEntries(){
		return orderedEntries;
}
/*
void Scope::updateIdValue(string id_,ValueHolder newVal_){
		if( idExistsInScope(id_) ){
				orderedEntries[this->identifiersLookUp[id_]].val=newVal_;
				orderedEntries[this->identifiersLookUp[id_]].valueKnown=true;
		}
		else
				parentScope->updateIdValue(id_,newVal_);
}*/
/*
void Scope::cancelIdValue(string id_){
		if( idExistsInScope(id_) )
				orderedEntries[this->identifiersLookUp[id_]].valueKnown=false;

		else
				parentScope->cancelIdValue(id_);

}*/
/*
void Scope::updateIdTempValue(string id_, string tempVal){
		if( idExistsInScope(id_) ){
				orderedEntries[this->identifiersLookUp[id_]].tempVal=tempVal;
		}
		else
				parentScope->updateIdTempValue(id_,tempVal);
}*/
/*
void Scope::cancelIdTempValue(string id_){
		if( idExistsInScope(id_) )
				orderedEntries[this->identifiersLookUp[id_]].tempVal=id_;
		else
				parentScope->cancelIdTempValue(id_);
}
*/
