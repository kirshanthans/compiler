#include "compiler.h"

int main(int argc, char **argv){
		Logger logger(4, "test.log"); 
		
		yyin = fopen(argv[1], "r");
		
		TOKEN token = static_cast<TOKEN> (yylex()); 
		
		while(token){
				if(token == t_ILLEGAL){
						logger.log("Illegal Token Captured");
						return -1;
				}
				cout << "Token Type: " << getTokenName(token) << endl;
				cout << "Value: "      << yytext              << endl;

				token = static_cast<TOKEN> (yylex());
		}

		return 0; 
}

string getTokenName(TOKEN token)
{
		switch(token){
				case(t_EOF):
						return "EOF"; 
				case(t_KEYWORD):
						return "KEYWORD"; 
				case(t_OPERATOR):
						return "OPERATOR";
				case(t_COMMENT):
						return "COMMENT";
				case(t_STRINGLITERAL):
						return "STRINGLITERAL";
				case(t_IDENTIFIER):
						return "IDENTIFIER";
				case(t_FLOATLITERAL):
						return "FLOATLITERAL";
				case(t_INTLITERAL):
						return "INTLITERAL";
				case(t_WHITESPACE):
						return "WHITESPACE";
				case(t_ILLEGAL):
						return "ILLEGAL";
				default:
						return "UNKNOWN";
		}	
}

