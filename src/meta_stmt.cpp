#include "../include/meta_stmt.h"

#include<iostream>
#include<cstdlib>

namespace miniDB{


void MetaSTMT::MetaCommandDispatch(){
    if(metaCommand.compare(".exit") == 0){
        MetaExit();
    } else if (metaCommand.compare(".help") == 0){
        MetaHelp();
    } else {
        std::cout << "Unrecognized Meta-STMT" << std::endl;
    }
    
}

void MetaSTMT::MetaExit(){
    exit(EXIT_SUCCESS);
}

void MetaSTMT::MetaHelp(){
    std::cout << "print help msg" << std::endl;
}













    
} // namespace miniDB


