//
// Created by Walker Crouse on 2018-12-19.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include "Connection.h"
#include "tuna.h"

using std::vector;

////////////////////////////////////////
///                                  ///
///         == Connection ==         ///
///                                  ///
////////////////////////////////////////

Connection::Connection(int fd, int buffer_size) : fd(fd), buffer_size(buffer_size), status(Status::Ok) {}

/// * Public methods * ///

void Connection::send(string msg) {
    size_t len = msg.size();
    ::send(fd, &len, sizeof(size_t), 0);
    ::send(fd, msg.c_str(), msg.size(), 0);
    printf("[local] %s\n", msg.c_str());
}

ssize_t Connection::recv(string &buffer) {
    size_t len;
    ::recv(fd, &len, sizeof(size_t), 0);
    vector<char> buff;
    buff.resize(len, 0x00);
    ssize_t size = ::recv(fd, &buff[0], len, 0);
    buffer.assign(&buff[0], buff.size());
    printf("[remote] %s\n", buffer.c_str());
    return size;
}

bool Connection::close() {
    int res = ::close(fd);
    if (res == 0) {
        status = Status::Closed;
        return true;
    }
    return false;
}

bool Connection::shutdown() {
    int res = ::shutdown(fd, SHUT_RDWR);
    if (res == 0) {
        status = Status::Shutdown;
        return true;
    }
    return false;
}

void Connection::setThread(shared_ptr<thread> th) {
    this->th = th;
}

shared_ptr<thread> Connection::getThread() const {
    return th;
}

void Connection::setUser(UserPtr user) {
    this->user = user;
}

UserPtr Connection::getUser() const {
    return user;
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
