#ifndef __SQL_STMT_H_
#define __SQL_STMT_H_


#include "defs.h"

namespace miniDB{


class SqlSTMT{

    public:
        SqlSTMT(std::string &stmt) : sqlCommand(stmt) {}
        void SQLCommandDispatch();
        void SQLInsert();
        void SQLSelect();

    private:
        std::string sqlCommand;

};







} // namespace miniDB

#endif