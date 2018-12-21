//
// Created by Walker Crouse on 2018-12-18.
//

#ifndef TUNAVID_VIDEOCLIENT_H
#define TUNAVID_VIDEOCLIENT_H

#include <string>
#include "TunaVid.h"
#include "../Connection.h"

using std::string;

/**
 * Represents a client capable of connecting to the VideoService.
 */
class VideoClient {

    string host;
    int port;
    int status;
    Ui::TunaVid ui;
    ConnPtr conn;

    /// callback method invoked when the login button is clicked
    void handleLogin(string username, string password);

public:

    /**
     * Starts the client.
     *
     * @param argc program argument count
     * @param argv program arguments
     * @return status code
     */
    int start(int argc, char *argv[]);

    bool refreshUserList();

    /**
     * Returns the current status code for the client.
     */
    int getStatus() const;

};

#endif //TUNAVID_VIDEOCLIENT_H
