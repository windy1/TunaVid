//
// Created by Walker Crouse on 2018-12-18.
//

#include "VideoService.h"
#include "../Connection.h"
#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <thread>
#include <sstream>

using std::thread;
using std::stringstream;

////////////////////////////////////////
///                                  ///
///        == VideoService ==        ///
///                                  ///
////////////////////////////////////////

/// * Public methods * ///

int VideoService::start(int port, int backlog) {
    printf("[[starting service]]\n");
    printf("port = %d\n", port);

    if ((status = init(port, backlog)) != STATUS_OK) return status;

    sockaddr_in cli_addr{};
    int addr_len = sizeof(cli_addr);
    int cli_socket;
    vector<thread> threads;

    running = true;
    while (running) {
        cli_socket = accept(socket_fd, (sockaddr *) &cli_addr, (socklen_t *) &addr_len);
        if (cli_socket < 0) {
            fprintf(stderr, "failed to accept new connection");
            continue;
        }
        ConnPtr conn = std::make_shared<Connection>(cli_socket);
        connections.push_back(conn);
        threads.push_back(thread(&VideoService::handleConnection, this, conn));
    }

    return status;
}

UserPtr VideoService::authenticate(ConnPtr conn) {
    string login;
    conn->recv(login);
    stringstream in(login);
    string token;
    string username;
    string password;
    for (int i = 0; std::getline(in, token, ' '); i++) {
        if (i == 0 && token != MSG_LOGIN) {
            conn->send(MSG_UNAUTHORIZED);
            conn->close();
            return nullptr;
        }
        if (i == 1) username = token;
        if (i == 2) password = token;
    }

    // this is where the password would actually be checked

    UserPtr user = getUser(username);
    if (user == nullptr) {
        user = std::make_shared<User>(username);
        users.push_back(user);
    }

    conn->send(MSG_AUTHORIZED);

    return user;
}

UserPtr VideoService::getUser(const string &username) const {
    for (auto &user : users) {
        if (user->getUsername() == username) {
            return user;
        }
    }
    return nullptr;
}

void VideoService::sendUserList(ConnPtr conn) {
    string msg = string(MSG_LIST) + " ";
    int len = users.size();
    for (int i = 0; i < len; i++) {
        msg += users[i]->getUsername();
        if (i < len - 1) msg += ',';
    }
    printf("sending user list %s\n", msg.c_str());
    conn->send(msg);
}

int VideoService::getStatus() const {
    return status;
}

/// * Private methods * ///

void VideoService::handleConnection(ConnPtr conn) {
    UserPtr user = authenticate(conn);
    if (user == nullptr) return;
    printf("user %s logged in", user->getUsername().c_str());
    while (conn->getStatus() != STATUS_SHUTDOWN) {
        string cmd;
        conn->recv(cmd);
        if (cmd == MSG_LIST) {
            sendUserList(conn);
        }
    }
}

int VideoService::init(int port, int backlog) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "failed to open socket\n");
        return STATUS_ERR_SOCKET;
    }
    if (::bind(socket_fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
        fprintf(stderr, "failed to bind socket\n");
        return STATUS_ERR_SOCKET;
    }
    if (listen(socket_fd, backlog) < 0) {
        fprintf(stderr, "failed to listen on socket\n");
        return STATUS_ERR_SOCKET;
    }
    printf("Accepting new connections...");
    return STATUS_OK;
}
