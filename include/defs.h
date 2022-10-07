#ifndef __DEFS_H_
#define __DEFS_H_


#include<string>

struct InputBuffer{
    std::string buffer;
};



enum CommandType{
    MetaCommand,
    SQLCommands,
};





#endif