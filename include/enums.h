#ifndef __ENUMS_H_
#define __ENUMS_H_


#include <string>

#include<syslog.h>


namespace minikv {
    


enum StatusCode{
    STATUS_OK,
    STATUS_ERR,

};


enum LogLevel {
    _LOG_ERR   = 3,
    _LOG_INFO  = 6,
    _LOG_DEBUG = 7,
    _LOG_USER  = 8,
};







} // namespace minikv



#endif