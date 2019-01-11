//
// Created by Walker Crouse on 2018-12-18.
//

#ifndef TUNAVID_TUNA_H
#define TUNAVID_TUNA_H

#include <string>

using std::string;

struct Status {

    static const int Ok;
    static const int Shutdown;
    static const int Closed;
    static const int SocketErr;
    static const int InvalidArgs;
    static const int LoginErr;

};

struct Message {

    static const string Login;
    static const string Unauthorized;
    static const string Authorized;
    static const string List;
    static const string Disconnect;
    static const string Goodbye;

};

#endif //TUNAVID_TUNA_H
