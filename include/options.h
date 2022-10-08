#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#include <string>
#include "enums.h"

namespace minikv {


struct LogOption{
    std::string logTarget;
    LogLevel    logLevel;
    
    static LogOption*      genDefaultLogOption();
    static void            PrintConfig(LogOption*);
};


struct DatabaseOption{
    std::string databaseName;
    std::string databasePath;

    static DatabaseOption* genDefaultDatabaseOption();
    static void            PrintOption(DatabaseOption*);
};

struct GetOption{
    
};

struct PutOption{

};













}

#endif