#include "../include/shell.h"

#include<iostream>

#include "../include/meta_stmt.h"
#include "../include/sql_stmt.h"


namespace miniDB{


void Shell::Loop(){

    while(true){
        PrintPrompt();
        std::string commandline;
        getline(std::cin, commandline);

        switch (CommandDispatch(commandline)) {
            case MetaCommand: {
                MetaSTMT metaSTMT(commandline);
                metaSTMT.MetaCommandDispatch();
                break;
            }
            case SQLCommands: {
                SqlSTMT sqlSTMT(commandline);
                sqlSTMT.SQLCommandDispatch();
                break;
            }
        } 
    }
}


void Shell::PrintPrompt(){
    std::cout << "> ";
}


CommandType Shell::CommandDispatch(std::string &command){
    return (command.at(0) == '.') ? MetaCommand : SQLCommands;
}


} // namespace miniDB
