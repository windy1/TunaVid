//
// Created by Walker Crouse on 2018-12-19.
//

#ifndef TUNAVID_CONNECTION_H
#define TUNAVID_CONNECTION_H

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <vector>

using std::string;
using std::shared_ptr;
using std::thread;
using std::mutex;
using std::vector;

class User;
class Connection;

typedef shared_ptr<Connection> ConnPtr;
typedef std::shared_ptr<User> UserPtr;

/**
 * Represents a socket-based connection.
 */
class Connection {

    static int LastId;

    int id;
    int socket_fd;
    int status;
    bool is_log_enabled;
    UserPtr user;
    string remoteTag;
    shared_ptr<thread> th;
    mutex write_mutex;
    mutex read_mutex;

    void _send(const string &msg);

    void recvFull(string &out, size_t len);

public:

    Connection(int fd);

    /**
     * Sends the specified string message to the other party.
     *
     * @param msg message to send
     */
    void send(const string &msg);

    /**
     * Sends the specified messages in the given vector to the other party. This
     * method ensures that each method is sent in succession without any
     * messages be sent in between in a thread-safe manner.
     *
     * @param data the data to send
     */
    void sendMulti(const vector<string> &data);

    /**
     * (Blocking) Receives a message from the other party.
     *
     * @param out buffer for incoming data
     * @return size of received data
     */
    bool recv(string &out);

    /**
     * Closes this connection.
     *
     * @return true if connection was closed successfully
     */
    bool close();

    /**
     * Shuts down all incoming and outgoing messages on this connection.
     *
     * @return true if successful
     */
    bool shutdown();

    /**
     * Sets the thread this connection is associated with.
     *
     * @param th thread
     */
    void setThread(shared_ptr<thread> th);

    /**
     * Sets the User this Connection is associated with
     *
     * @param user to set
     */
    void setUser(UserPtr user);

    /**
     * Sets whether this connection should print incoming and outgoing messages
     * to stdout.
     *
     * @param is_log_enabled true if should print to stdout
     */
    void setLogEnabled(bool is_log_enabled);

    /**
     * Returns the thread this connection is associated with.
     *
     * @return connection thread
     */
    shared_ptr<thread> getThread() const;

    /**
     * Returns the User this Connection is associated with.
     *
     * @return user
     */
    UserPtr getUser() const;

    /**
     * Returns whether this connection should print incoming and outgoing
     * messages to stdout.
     *
     * @return true if should print to stdout
     */
    bool isLogEnabled() const;

    /**
     * Returns the current status code for this connection.
     *
     * @return current status code
     */
    int getStatus() const;

    /**
     * Returns this connections unique identifier.
     *
     * @return connection id
     */
    int getId() const;

    /**
     * Connects to the specified host and port and returns newly initialized Connection instance.
     *
     * @param host to connect to
     * @param port to connect to
     * @return pointer to Connection instance if successful, null otherwise
     */
    static ConnPtr connect(string &host, int port);

};

#endif //TUNAVID_CONNECTION_H
