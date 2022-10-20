#include "../include/status.h"


namespace minikv {



bool Status::IsOK(){
    return _statusCode == STATUS_OKAY;
}

std::string Status::GetStatusCodeStr(StatusCode code){
    switch(code) {
        case STATUS_OKAY:
            return "STATUS_OKAY";
        case STATUS_ERROR:
            return "STATUS_ERROR";
        case STATUS_FILE_NOT_OPEN:
            return "STATUS_FILE_NOT_OPEN";
        case STATUS_EMPTY_PTR:
            return "STATUS_EMPTY_PTR";
        case STATUS_NO_SPACE:
            return "STATUS_NO_SPACE";
        case STATUS_NOT_FOUND:
            return "STATUS_NOT_FOUND";
        case STATUS_IO_ERROR:
            return "STATUS_IO_ERROR";
        case STATUS_BUFFER_FULL:
            return "STATUS_BUFFER_FULL";
        case STATUS_STR_TOO_LONG:
            return "STATUS_STR_TOO_LONG";
        default:
            return "";
    }
}


std::string Status::ToString(){
    return "[Code]:" + GetStatusCodeStr(_statusCode) + ";\t[Msg]:" + _statusMessage;
}


}