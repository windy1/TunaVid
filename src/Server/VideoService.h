//
// Created by Walker Crouse on 2018-12-18.
//

#ifndef TUNAVID_VIDEOSERVICE_H
#define TUNAVID_VIDEOSERVICE_H

#include "../tuna.h"
#include "../Connection.h"
#include "User.h"
#include <vector>

using std::vector;

/**
 * Main class for video-streaming service. This service acts as an intermediary station for data exchanged between
 * multiple clients.
 */
class VideoService {

    int status;
    int socket_fd;
    int running;

    vector<ConnPtr> connections;
    vector<UserPtr> users;

    /// handles / processes new connection to the server
    void handleConnection(ConnPtr conn);

    /// performs initialization logic during startup
    int init(int port, int backlog);

public:

    /**
     * Starts the server on the specified port with the specified amount of backlog connections.
     *
     * @param port server port
     * @param backlog connection to allow in the backlog
     * @return status code
     */
    int start(int port, int backlog=10);

    /**
     * Attempts to authenticate the specified connection.
     *
     * @param conn connection to authenticate
     * @return user pointer of authenticated user or null if authentication failed
     */
    UserPtr authenticate(ConnPtr conn);

    /**
     * Returns the User with the specified username. If the user is not online, null is returned.
     *
     * @param username to lookup
     * @return User if online, null otherwise
     */
    UserPtr getUser(const string& username) const;

    /**
     * Returns all currently opened connection.
     *
     * @return open connections
     */
    const vector<ConnPtr>& getConnections() const;

    /**
     * Sends the user list to the specified Connection.
     *
     * @param conn connection to send user list to
     */
    void sendUserList(ConnPtr conn);

    /**
     * Disconnects the specified connection. If this is the last connection
     * for the associated user, the user will be removed from the user list.
     *
     * @param conn connection to disconnect
     * @param close whether the socket should be closed or just shutdown
     */
    void disconnect(ConnPtr conn, bool close = false);

    /**
     * Returns the current service status code.
     *
     * @return status code
     */
    int getStatus() const;

};

#endif //TUNAVID_VIDEOSERVICE_H
