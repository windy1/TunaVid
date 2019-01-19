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

};

struct Message {

    static const string Login;
    static const string Unauthorized;
    static const string Authorized;
    static const string List;
    static const string Disconnect;
    static const string Join;
    static const string Leave;
    static const string Frame;
    static const string Call;
    static const string CallInvalid;
    static const string CallWaiting;
    static const string CallIncoming;
    static const string CallAccept;
    static const string CallIgnore;
    static const string CallOpen;
    static const string CallClose;
    static const string Goodbye;

};

#endif //TUNAVID_TUNA_H
