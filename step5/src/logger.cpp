#include"logger.h"

Logger::Logger(int mode_,string fileName_, string header_)
{
		this->mode=mode_; 
		this->header=header_;
		if(mode_ != _LOGGER_OFF_MODE){
				this->file.open(fileName_.c_str(), std::ofstream::out | std::ofstream::app);
				this->log("Logger Initialized");
		}    
}

Logger::~Logger(){
		if(this->mode !=_LOGGER_OFF_MODE)
				if(this->file.is_open())
						this->file.close(); 
}

void Logger::log(string s){

		if(this->mode == _LOGGER_FILE_MODE ||    this->mode == _LOGGER_HYBRID_MODE)
				this-> file << this->header+":"<<"\t"<<s<<endl;

		if(this->mode == _LOGGER_STDOUT_MODE || this-> mode == _LOGGER_HYBRID_MODE)
				cout << this->header << " :\t" << s << endl; 
}
