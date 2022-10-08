#include "../include/status.h"

#include <iostream>

namespace minikv {


StatusCode Status::GetStatusCode(){
    return statusCode;
}

void Status::Print(){
    std::cout << "[statusCode]:\t" << this->statusCode << std::endl;
    std::cout << "[statusMsg ]:\t" << this->statusMessage << std::endl;
    
}



}