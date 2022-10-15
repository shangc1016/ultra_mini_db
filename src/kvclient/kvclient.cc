#include "../../include/storageengine.h"
#include "../../include/utils.h"


std::string key;
std::string value;
std::string valuePlaceHolder;

minikv::StorageEngine engine;

minikv::Status *status;



// Read-> Eval-> Print Loop
void REPL() {

    for(;;){
        std::cout << "> ";
        std::cout << "[PUT] key: ";
        std::cin >> key;
        if(key.compare(".exit") == 0){

            exit(EXIT_SUCCESS);
        }
        std::cout << "[PUT] value: ";
        std::cin >> value;
        // PUT
        status = engine.Put(key, value);
        if(!status->IsOK()) {
            std::cout << status->ToString() << std::endl;
        }
        delete status;

        // GET
        status = engine.Get(key, &valuePlaceHolder);
        if(!status->IsOK()) {
            std::cout << status->ToString() << std::endl;
        }
        delete status;
        std::cout << "[GET] key: " << key <<";\tvalue: " << valuePlaceHolder << std::endl;
    }
}


// kclient main
int main() {

    REPL();

    return 0;
}