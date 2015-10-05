#include "compiler.h"

int main(int argc, char **argv){
		Logger logger(4, "test.log"); 
		
		yyin = fopen(argv[1], "r");
		
		int parse = yyparse();
		
		
		return 0; 
}
