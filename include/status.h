#ifndef __STATUS_H_
#define __STATUS_H_

#include <string>
#include <iostream>

namespace minikv {

enum StatusCode{
    STATUS_OKAY,
    STATUS_ERROR,
    STATUS_FILE_NOT_OPEN,
    STATUS_EMPTY_PTR,
};


class Status {

public:

    Status(StatusCode _code, std::string _msg) : _stateCode(_code), _statusMessage(_msg) {};
    
    void Check(){
        if(this->_stateCode != STATUS_OKAY)
            this->Print();
    }
    
    void Print() {
        std::cout << "[Code]: " << this->_stateCode << ";\t[Msg]: " << this->_statusMessage << std::endl;
    }

    StatusCode  _stateCode;
    std::string _statusMessage;

};


}

#endif