#ifndef __LOG_H_
#define __LOG_H_

#include <string>
#include <sys/syslog.h>
#include <syslog.h>
#include <unistd.h>

namespace minikv {


enum LogLevel{
    LogDebug,
    LogInfo ,
    LogWarn ,
    LogError,
    LogEmerg,
    LogTrace,
};


// 大于当前currentLogLevel的日志才能被写到syslog文件

class Logger{

public:

    static void Debug(std::string);
    
    static void Info(std::string);

    static void Warning(std::string);

    static void Error(std::string);

    static void Emerg(std::string);

    static void Trace(std::string, std::string);

    static void Log(int, std::string);

    static void SetLogLevel(int);

    static int GetLogLevel();

    static int GenFacility();

    static int GenOption();



private:
    // 是不是线程安全的？
    static int currentLogLevel;
    static std::string ident;
    static int facility;
    static int option;


};






}


#endif