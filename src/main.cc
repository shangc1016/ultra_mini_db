#include <iostream>
#include <string>
#include <mutex>
#include <thread>

#include "../include/storageengine.h"


int main() {


    minikv::StorageEngine engine;

    std::string key;
    std::string value;

    
    minikv::Status status = engine.Put("s", "sc");
    if(status._stateCode != minikv::STATUS_OKAY) {
        status.Print();
    }
    minikv::Status s1 = engine.Get("s", &value);
    if(s1._stateCode != minikv::STATUS_OKAY){
        status.Print();
    } else {
        std::cout << "value = " << value << std::endl;
    }

    
    
    return 0;

}