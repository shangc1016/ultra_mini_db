#ifndef __STATUS_H_
#define __STATUS_H_

#include<string>

#include "enums.h"


namespace minikv {

class Status{

public:
    Status(StatusCode code_, std::string msg): statusCode(code_), statusMessage(msg) {};
    void Print();
    StatusCode GetStatusCode();

private:
    enum StatusCode  statusCode;
    std::string statusMessage;

};


}

#endif