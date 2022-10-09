#include <sys/syslog.h>
#include <syslog.h>

#include "../include/logger.h"
#include "../include/status.h"



namespace minikv {

Logger::Logger() {}

Logger::~Logger() {}

LogOption* LogOption::genDefaultLogOption(){
    LogOption *option = new LogOption();
    option->logLevel = _LOG_DEBUG;
    option->logTarget = "minikv";
    return option;
}


void Logger::Log(LogLevel level, std::string logStr){
    std::string label;
    LogOption *logOption = LogOption::genDefaultLogOption();
    if(level > logOption->logLevel) {
        openlog(logOption->logTarget.c_str(), LOG_CRON |LOG_PID, 0);
        syslog(level, "%s", logStr.c_str());
        closelog();
    }
}



void Logger::INFO (std::string str){
    Logger::Log(_LOG_INFO, str);
}

void Logger::DEBUG(std::string str){
    Logger::Log(_LOG_DEBUG, str);
}



}