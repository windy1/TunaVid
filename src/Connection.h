//
// Created by Walker Crouse on 2018-12-19.
//

#ifndef TUNAVID_CONNECTION_H
#define TUNAVID_CONNECTION_H

#include <string>
#include <memory>
#include <thread>

using std::string;
using std::shared_ptr;
using std::thread;

class User;
class Connection;

typedef shared_ptr<Connection> ConnPtr;
typedef std::shared_ptr<User> UserPtr;

/**
 * Represents a socket-based connection.
 */
class Connection {

    int fd;
    int buffer_size;
    int status;
    UserPtr user;
    shared_ptr<thread> th;

public:

    Connection(int fd, int buffer_size=1024);

    /**
     * Sends the specified string message to the other party.
     *
     * @param msg message to send
     */
    void send(string msg);

    /**
     * (Blocking) Receives a message from the other party.
     *
     * @param buffer buffer for incoming data
     * @return size of received data
     */
    ssize_t recv(string &buffer);

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
     * Returns the thread this connection is associated with.
     *
     * @return connection thread
     */
    shared_ptr<thread> getThread() const;

    /**
     * Sets the User this Connection is associated with
     *
     * @param user to set
     */
    void setUser(UserPtr user);

    /**
     * Returns the User this Connection is associated with.
     *
     * @return user
     */
    UserPtr getUser() const;

    /**
     * Returns the buffer size of this connection.
     *
     * @return buffer size
     */
    int getBufferSize() const;

    /**
     * Returns the current status code for this connection.
     *
     * @return current status code
     */
    int getStatus() const;

    /**
     * Connects to the specified host and port and returns newly initialized Connection instance.
     *
     * @param host to connect to
     * @param port to connect to
     * @return pointer to Connection instance if successful, null otherwise
     */
    static ConnPtr connect(string host, int port);

};

#endif //TUNAVID_CONNECTION_H
