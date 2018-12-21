//
// Created by Walker Crouse on 2018-12-19.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Connection.h"
#include "tuna.h"

////////////////////////////////////////
///                                  ///
///         == Connection ==         ///
///                                  ///
////////////////////////////////////////

Connection::Connection(int fd, int buffer_size) : fd(fd), buffer_size(buffer_size), status(STATUS_OK) {}

/// * Public methods * ///

void Connection::send(string msg) {
    ::send(fd, msg.c_str(), msg.size(), 0);
}

ssize_t Connection::recv(string &buffer) {
    char buff[buffer_size];
    ssize_t size = ::recv(fd, buff, buffer_size, 0);
    buffer = buff;
    return size;
}

bool Connection::close() {
    return ::close(fd) < 0;
}

int Connection::getBufferSize() const {
    return buffer_size;
}

int Connection::getStatus() const {
    return status;
}

/// * Static methods * ///

ConnPtr Connection::connect(string host, int port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "failed to open socket\n");
        return nullptr;
    }
    if (host == "localhost") host = "127.0.0.1";
    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        fprintf(stderr, "invalid host address\n");
        return nullptr;
    }
    if (::connect(fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
        fprintf(stderr, "connection failed\n");
        return nullptr;
    }
    return std::make_shared<Connection>(fd);
}
