#ifndef __SHELL_H_
#define __SHELL_H_

#include<string>

#include"defs.h"

namespace miniDB{

class Shell {

    public:
        void Loop();
        void PrintPrompt();
        CommandType CommandDispatch(std::string &);
};



















};

#endif