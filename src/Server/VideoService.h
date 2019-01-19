//
// Created by Walker Crouse on 2018-12-18.
//

#ifndef TUNAVID_VIDEOSERVICE_H
#define TUNAVID_VIDEOSERVICE_H

#include "../tuna.h"
#include "../Connection.h"
#include "../User.h"
#include "CallSession.h"
#include "ServerMonitor.h"
#include <vector>
#include <thread>
#include <map>
#include <functional>
#include <sstream>

using std::vector;
using std::thread;
using std::map;
using std::function;
using std::stringstream;

/**
 * Main class for video-streaming service. This service acts as an intermediary station for data exchanged between
 * multiple clients.
 */
class VideoService {

    int status;
    int socket_fd;
    bool is_running;
    thread listen_thread;

    vector<ConnPtr> connections;
    vector<UserPtr> users;
    vector<CallSessionPtr> call_sessions;
    map<string, function<void(ConnPtr, stringstream&)>> handlerMap;

    ServerMonitor monitor;

    int init(int port, int backlog);

    void initHandlers();

    void startListening();

    void handleConnection(ConnPtr conn);

    void _sendAll(const string &message);

    void onList(ConnPtr conn, stringstream &in);
    void onCall(ConnPtr conn, stringstream &in);
    void onCallAccept(ConnPtr conn, stringstream &in);
    void onCallIgnore(ConnPtr conn, stringstream &in);
    void onFrame(ConnPtr conn, stringstream &in);
    void onDisconnect(ConnPtr conn, stringstream &in);

public:

    VideoService();

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
     * Sends the specified message to every authenticated connection currently
     * connected to the server. This method is completed asynchronously.
     *
     * @param message to send
     */
    void sendAll(const string &message);

    /**
     * Returns all currently opened connection.
     *
     * @return open connections
     */
    const vector<ConnPtr>& getConnections() const;

    /**
     * Returns all active CallSessions.
     *
     * @return active CallSessions
     */
    const vector<CallSessionPtr>& getCallSessions() const;

    /**
     * Returns the User with the specified username. If the user is not online, null is returned.
     *
     * @param username to lookup
     * @return User if online, null otherwise
     */
    UserPtr getUser(const string& username) const;

    /**
     * Returns the active CallSession with the specified ID. If there is no
     * call with the specified ID, null is returned.
     *
     * @param callId unique ID
     * @return call with specified id
     */
    CallSessionPtr getCall(int callId) const;

    /**
     * Returns true if the server is currently running.
     *
     * @return true if running
     */
    bool isRunning() const;

    /**
     * Returns the current service status code.
     *
     * @return status code
     */
    int getStatus() const;

};

#endif //TUNAVID_VIDEOSERVICE_H
