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

int Connection::LastId = -1;

Connection::Connection(int fd)
    : socket_fd(fd),
      status(Status::Ok),
      remoteTag("remote"),
      id(++LastId),
      is_log_enabled(true) {}

/// * Public methods * ///

void Connection::send(const string &msg) {
    std::lock_guard lock(write_mutex);
    _send(msg);
}

void Connection::sendMulti(const vector<string> &data) {
    std::lock_guard lock(write_mutex);
    for (auto &str : data) _send(str);
}

bool Connection::recv(string &out) {
    std::lock_guard lock(read_mutex);
    out = "";
    // get the size of the incoming message
    size_t len;
    if (::recv(socket_fd, &len, sizeof(size_t), 0) < 0) {
        perror("the message length could not be received");
        return false;
    }
    // receive the message
    recvFull(out, len);
    if (!out.empty() && is_log_enabled) {
        printf("[%s] %s\n", remoteTag.c_str(), out.c_str());
    }
    return true;
}

bool Connection::close() {
    if (::close(socket_fd) < 0) {
        perror("failed to close socket");
        return false;
    }
    status = Status::Closed;
    return true;
}

bool Connection::shutdown() {
    if (::shutdown(socket_fd, SHUT_RDWR) < 0) {
        perror("failed to shutdown socket");
        return false;
    }
    status = Status::Shutdown;
    return false;
}

/// * Private methods * ///

void Connection::_send(const string &msg) {
    if (msg.empty()) {
        fprintf(stderr, "error: tried to send an empty message\n");
        return;
    }
    size_t len = msg.size();
    ::send(socket_fd, &len, sizeof(size_t), 0);
    ::send(socket_fd, msg.c_str(), msg.size(), 0);
    if (!msg.empty() && is_log_enabled) {
        printf("[local => %s] %s\n", remoteTag.c_str(), msg.c_str());
    }
}

void Connection::recvFull(string &out, size_t len) {
    size_t bytes_left = len;
    vector<char> buffer;
    buffer.resize(len, 0x00);
    while (bytes_left > 0) {
        ssize_t n = ::recv(socket_fd, &buffer[0], bytes_left, 0);
        if (n < 0) {
            perror("failed to receive message");
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

/// * Setters * ///

void Connection::setThread(shared_ptr<thread> th) {
    this->th = th;
}

void Connection::setUser(UserPtr user) {
    this->user = user;
    this->remoteTag = user->getUsername();
}

void Connection::setLogEnabled(bool is_log_enabled) {
    this->is_log_enabled = is_log_enabled;
}

/// * Getters * ///

shared_ptr<thread> Connection::getThread() const {
    return th;
}

UserPtr Connection::getUser() const {
    return user;
}

bool Connection::isLogEnabled() const {
    return is_log_enabled;
}

int Connection::getStatus() const {
    return status;
}

int Connection::getId() const {
    return id;
}
