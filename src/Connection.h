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

class Connection {

    int fd;

public:

    Connection(int fd);

    void send(string msg);

    ssize_t recv(char *buffer, size_t buffer_size);

    bool close();

    static ConnPtr connect(string host, int port);

};

#endif //TUNAVID_CONNECTION_H
