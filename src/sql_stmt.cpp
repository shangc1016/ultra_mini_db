#include "../include/sql_stmt.h"

#include<iostream>


namespace miniDB{

void SqlSTMT::SQLCommandDispatch(){
    if(sqlCommand.find("insert", 0) == 0) {
        SQLInsert();
    } else if(sqlCommand.find("select", 0) == 0) {
       SQLSelect();
    } else {
       std::cout << "Unrecognized SQL-STMT" << std::endl;
    }
}


void SqlSTMT::SQLInsert(){
    std::cout << "insert... [" << sqlCommand << "]" << std::endl;
}
void SqlSTMT::SQLSelect(){
    std::cout << "select... [" << sqlCommand << "]" << std::endl;
} 








} // namespace miniDB