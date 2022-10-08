#include <sys/syslog.h>
#include <syslog.h>

#include "../include/logger.h"
#include "../include/status.h"



namespace minikv {

Logger::Logger(){
    this->logOption = *LogOption::genDefaultLogOption();
}

Logger::~Logger() {}

LogOption* LogOption::genDefaultLogOption(){
    LogOption *option = new LogOption();
    option->logLevel = _LOG_DEBUG;
    option->logTarget = "minikv";
    return option;
}

void LogOption::PrintConfig(LogOption *option){
    std::cout << "[logLevel ]:\t" << option->logLevel  << std::endl;
    std::cout << "[logTarget]:\t" << option->logTarget << std::endl;
}

void Logger::Log(LogLevel level, std::string logStr){
    std::string label;
    switch(this->logOption.logLevel){
        case _LOG_DEBUG : 
            label = "LOG_DEBUG";
            break;
        case _LOG_INFO:
            label = "LOG_INFO";
            break;
        case _LOG_ERR:
            label = "LOG_ERR";
            break;
        case _LOG_USER:
            label = "LOG_USER";
            break;
    }
    label = this->logOption.logTarget + "-" + label;
    if(level >= this->logOption.logLevel) {
        openlog(label.c_str(), LOG_CRON | LOG_PID, 0);
        syslog(level, "%s", logStr.c_str());
        closelog();
    }
}

void Logger::SetLogLevel(LogLevel level){
    this->logOption.logLevel = level;
}

void Logger::INFO (std::string str){
    this->Log(_LOG_INFO, str);
}

void Logger::DEBUG(std::string str){
    this->Log(_LOG_DEBUG, str);
}



}