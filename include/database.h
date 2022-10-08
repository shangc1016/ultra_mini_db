#ifndef __DATABASE_H_
#define __DATABASE_H_


#include <iostream>

#include "status.h"
#include "options.h"


namespace minikv {


class Database {

public:
    Database();
    Database(DatabaseOption *option) : databaseOption(*option) {};

    ~Database() {
        std::cout << "database deconstructor" << std::endl;
    }
    Status* Get();
    Status* Put();
    Status* Delete();

private:

    DatabaseOption databaseOption;

};





}

#endif