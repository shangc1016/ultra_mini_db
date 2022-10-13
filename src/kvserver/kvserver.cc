#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <vector>

#include "../../include/storageengine.h"
#include "../../include/utils.h"
#include "../../include/hash.h" 
#include "../../include/log.h"



using namespace minikv;

// kvservber main
int main() {

    std::cout << Logger::GetLogLevel() << std::endl;

    Logger::Debug("std::string. debug");
    Logger::Info("std::string info");
    Logger::Warning("std::string, warning");
    Logger::Error("std::string, error");
    Logger::Emerg("std::string, emerg");



    return 0;
}

