//
// Created by Walker Crouse on 2018-12-19.
//

#ifndef TUNAVID_CONNECTION_H
#define TUNAVID_CONNECTION_H

#include <string>
#include <memory>

using std::string;
using std::shared_ptr;

class Connection;

typedef shared_ptr<Connection> ConnPtr;

/**
 * Represents a socket-based connection.
 */
class Connection {

    int fd;
    int buffer_size;
    int status;

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

    int getBufferSize() const;

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
