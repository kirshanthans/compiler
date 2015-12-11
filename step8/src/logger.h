#ifndef _LOGGER
#define _LOGGER

#include <string>
#include <iostream>
#include <fstream>

#define _LOGGER_FILE_MODE     1
#define _LOGGER_STDOUT_MODE   2 
#define _LOGGER_HYBRID_MODE   3
#define _LOGGER_OFF_MODE      4

using namespace std; 

class Logger
{
 private:
 	string header;
 	int mode;
 	ofstream file;
    

 public:
 	Logger(int mode_, string fileName_, string header="DEBUG");
 	void log(string s);
 	~Logger(); 
};


#endif
