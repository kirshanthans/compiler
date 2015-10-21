/*
 * main.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: lsakka
 */
#include"astnode.h"
#include"symboltable.h"
#include<iostream>
using namespace std;
int main(){
	Scope * global=createGlobalScope();
	global->addSymbolEntry("a",SymbolEntryType::FLOAT);
	global->addSymbolEntry("b",SymbolEntryType::FLOAT);
	global->addSymbolEntry("c",SymbolEntryType::FLOAT);
	fflush(stdout);

/*
	//c=6
	//b=5
	//a=b+c

	ASTNode * stList3=new StatementList(new AssignStatement(new Identifier("a",global),new Binop(new Identifier("b", global),new Identifier("c",global),ADD)),NULL);
	ASTNode * stList2=new StatementList(new AssignStatement(new Identifier("b",global),new Constant("5",INT)),stList3);
	ASTNode * stList=new StatementList(new AssignStatement(new Identifier("c",global),new Constant("6",INT)),stList2);

	vector<IRNode> v=stList->genCode();
	cout<<convertCodetoString(v);
*/

/*

	//b=3
	//c=1+b*188/100


	ASTNode * mul=new Binop(new Identifier("b",global),new Constant("188",INT),MUL);
	ASTNode * muldiv=new Binop(mul, new Constant("100",INT),DIV);
	ASTNode * add=new Binop(new Constant("1",INT),muldiv,ADD);
	ASTNode * assign=new AssignStatement(new Identifier("c",global),add);

	ASTNode * assign2=new AssignStatement(new Identifier("b",global),new Constant("3",INT));
	ASTNode * stList=new StatementList(assign2,assign);
	vector<IRNode> v=stList->genCode();

	cout<<convertCodetoString(v);
*/

/*

		//b=3
		//c=1.1+b*188/100


		ASTNode * mul=new Binop(new Identifier("b",global),new Constant("188",FLOAT),MUL);
		ASTNode * muldiv=new Binop(mul, new Constant("100",FLOAT),DIV);
		ASTNode * add=new Binop(new Constant("1.1",FLOAT),muldiv,ADD);
		ASTNode * assign=new AssignStatement(new Identifier("c",global),add);

		ASTNode * assign2=new AssignStatement(new Identifier("b",global),new Constant("3",FLOAT));
		ASTNode * stList=new StatementList(assign2,assign);
		vector<IRNode> v=stList->genCode();

		cout<<convertCodetoString(v)<<endl;
		cout<<"after:"<<endl;
		removedRedundentIRs(v);
		cout<<convertCodetoString(v);


		//b=3
		//c=1.1+b*188/100
*/

		ASTNode * mul=new Binop(new Identifier("b",global),new Constant("188",FLOAT),MUL);
		ASTNode * muldiv=new Binop(mul, new Constant("100",FLOAT),DIV);
		ASTNode * add=new Binop(new Constant("1.1",FLOAT),muldiv,ADD);
		ASTNode * assign=new AssignStatement(new Identifier("c",global),add);

		ASTNode * assign2=new AssignStatement(new Identifier("b",global),new Constant("3",FLOAT));
		ASTNode * stList=new StatementList(assign2,assign);
		vector<IRNode> v=stList->genCode();

		cout<<convertCodetoString(v)<<endl;
		cout<<"after:"<<endl;
		removedRedundentIRs(v);
		cout<<convertCodetoString(v);
}
