#include "../include/database.h"
#include "../include/status.h"

namespace minikv {

Database::Database(){
    this->databaseOption = *DatabaseOption::genDefaultDatabaseOption();
}


DatabaseOption* DatabaseOption::genDefaultDatabaseOption(){
    DatabaseOption *option = new DatabaseOption();
    option->databaseName = "minikv";
    option->databasePath = "minikvPath";
    return option;
}

void DatabaseOption::PrintOption(DatabaseOption *dbOption){
    std::cout << "[dbName]:\t" << dbOption->databaseName << std::endl;
    std::cout << "[dbPath]:\t" << dbOption->databasePath << std::endl;
}

Status* Database::Delete(){
    DatabaseOption::PrintOption(&this->databaseOption);
    return new Status(STATUS_OK, "hello");
}


}