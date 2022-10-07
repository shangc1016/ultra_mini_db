#ifndef __META_STMT_H_
#define __META_STMT_H_

#include<string>


namespace miniDB{

class MetaSTMT{

    public:
        MetaSTMT(std::string &stmt): metaCommand(stmt) {}

        void MetaCommandDispatch();
        void MetaExit();
        void MetaHelp();
    
    private:
        std::string metaCommand;

};













} // namespace miniDB
#endif