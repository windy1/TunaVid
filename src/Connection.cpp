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
#include "User.h"

using std::vector;

////////////////////////////////////////
///                                  ///
///         == Connection ==         ///
///                                  ///
////////////////////////////////////////

Connection::Connection(int fd) : fd(fd), status(Status::Ok), remoteTag("remote") {}

/// * Public methods * ///

void Connection::send(const string &msg) {
    write_mutex.lock();
    _send(msg);
    write_mutex.unlock();
}

void Connection::sendMulti(const vector<string> &data) {
    write_mutex.lock();
    for (auto &str : data) _send(str);
    write_mutex.unlock();
}

ssize_t Connection::recv(string &out) {
    read_mutex.lock();
    out = "";
    // get the size of the incoming message
    size_t len;
    ::recv(fd, &len, sizeof(size_t), 0);
    recvFull(out, len);
    if (!out.empty()) {
        printf("[%s] %s\n", remoteTag.c_str(), out.c_str());
    }
    read_mutex.unlock();
    return len;
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
    this->remoteTag = user->getUsername();
}

UserPtr Connection::getUser() const {
    return user;
}

int Connection::getStatus() const {
    return status;
}

/// * Private methods * ///

void Connection::_send(const string &msg) {
    size_t len = msg.size();
    ::send(fd, &len, sizeof(size_t), 0);
    ::send(fd, msg.c_str(), msg.size(), 0);
    printf("[local => %s] %s\n", remoteTag.c_str(), msg.c_str());
}

void Connection::recvFull(string &out, size_t len) {
    size_t bytes_left = len;
    vector<char> buffer;
    buffer.resize(len, 0x00);
    while (bytes_left > 0) {
        ssize_t n = ::recv(fd, &buffer[0], bytes_left, 0);
        if (n < 0) {
            fprintf(stderr, "error: Connection::recvFull\n");
            continue;
        }
        out.append(&buffer[0], (size_t) n);
        bytes_left -= n;
    }
}

/// * Static methods * ///

ConnPtr Connection::connect(string &host, int port) {
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
