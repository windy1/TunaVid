//
// Created by Walker Crouse on 2018-12-18.
//

#ifndef TUNAVID_VIDEOCLIENT_H
#define TUNAVID_VIDEOCLIENT_H

#include <string>
#include "TunaVid.h"

using std::string;

class VideoClient {

    string host;
    int port;
    int status;
    Ui::TunaVid ui;

    void handleLogin(string username, string password);

public:

    int start(int argc, char *argv[]);

    int getStatus() const;

};

#endif //TUNAVID_VIDEOCLIENT_H
