#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <iostream>

#include "enums.h"
#include "options.h"


namespace minikv {


// 是不是可以使用单例模式
// Logger只需要确定log的输出是什么
class Logger{

public:
    Logger();
    ~Logger();

    static void INFO (std::string);
    static void DEBUG(std::string);

private:

    static void Log(LogLevel, std::string);

};




}


#endif