#include "compiler.h"

int main(int argc, char **argv){
		Logger logger(4, "test.log"); 
		
		yyin = fopen(argv[1], "r");
		
		int token = yylex(); 
		
		while(token){
				if(token == TOKEN_ILLEGAL){
						logger.log("Illegal Token Captured");
						return -1;
				}
				cout << "Token Type: " << getTokenName(token) << endl;
				cout << "Value: "      << yytext              << endl;

				token = yylex();
		}

		return 0; 
}

string getTokenName(int token)
{
		switch(token){
				case(TOKEN_EOF):
						return "EOF"; 
				case(TOKEN_KEYWORD):
						return "KEYWORD"; 
				case(TOKEN_OPERATOR):
						return "OPERATOR";
				case(TOKEN_COMMENT):
						return "COMMENT";
				case(TOKEN_STRINGLITERAL):
						return "STRINGLITERAL";
				case(TOKEN_IDENTIFIER):
						return "IDENTIFIER";
				case(TOKEN_FLOATLITERAL):
						return "FLOATLITERAL";
				case(TOKEN_INTLITERAL):
						return "INTLITERAL";
				case(TOKEN_WHITESPACE):
						return "WHITESPACE";
				case(TOKEN_ILLEGAL):
						return "ILLEGAL";
				default:
						return "UNKNOWN";
		}	
}

