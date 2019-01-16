//
// Created by Walker Crouse on 2018-12-18.
//

#ifndef TUNAVID_VIDEOCLIENT_H
#define TUNAVID_VIDEOCLIENT_H

#include <string>
#include "TunaVid.h"
#include "../Connection.h"
#include "MessageListener.h"
#include "../User.h"

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
    MessageListener message_listener;
    UserPtr user;
    int activeCallId;
    int pendingCallId;

    /// callback method invoked when the login button is clicked
    void handleLogin(const string &username, const string &password);

    void handleCall(const string &receiver, CallAction action);

    void handleFrame(const string &data);

    // handles shutdown after the UI has been closed
    void shutdown();

public:

    VideoClient();

    /**
     * Starts the client.
     *
     * @param argc program argument count
     * @param argv program arguments
     * @return status code
     */
    int start(int argc, char *argv[]);

    void setUser(UserPtr user);

    void setActiveCallId(int callId);

    void setPendingCallId(int callId);

    ConnPtr getConnection() const;

    Ui::TunaVid& getUi();

    UserPtr getUser() const;

    int getActiveCallId() const;

    int getPendingCallId() const;

    const string& getHost() const;

    int getPort() const;

    /**
     * Returns the current status code for the client.
     */
    int getStatus() const;

};

#endif //TUNAVID_VIDEOCLIENT_H
