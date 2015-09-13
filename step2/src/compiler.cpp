#include "compiler.h"

int main(int argc, char **argv){
		Logger logger(4, "test.log"); 
		
		yyin = fopen(argv[1], "r");
		
		int parse = yyparse();
		
		if(parse)
				cout << "Not accepted" << endl;
		else
				cout << "Accepted" << endl;

		return 0; 
}

string getTokenName(int token)
{
		if(token >= TOKEN_KEY_PROGRAM && token <= TOKEN_KEY_FLOAT)
				return "KEYWORD"; 
		else if(token >= TOKEN_OP_ASSIGN   && token <= TOKEN_OP_COMMA)
				return "OPERATOR";
		else if(token == TOKEN_STRINGLITERAL)
				return "STRINGLITERAL";
		else if(token == TOKEN_IDENTIFIER)
				return "IDENTIFIER";
		else if(token == TOKEN_FLOATLITERAL)
				return "FLOATLITERAL";
		else if(token == TOKEN_INTLITERAL)
				return "INTLITERAL";
		else if(token == TOKEN_ILLEGAL)
				return "ILLEGAL";
		else
				return "UNKNOWN";
}
