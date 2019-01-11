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

    if ((status = init(port, backlog)) != Status::Ok) {
        return status;
    }

    sockaddr_in cli_addr{};
    int addr_len = sizeof(cli_addr);
    int cli_socket;

    // start accepting new connections
    running = true;
    while (running) {
        cli_socket = accept(socket_fd, (sockaddr *) &cli_addr, (socklen_t *) &addr_len);
        if (cli_socket < 0) {
            continue;
        }
        ConnPtr conn = std::make_shared<Connection>(cli_socket);
        connections.push_back(conn);
        shared_ptr<thread> th = std::make_shared<thread>(&VideoService::handleConnection, this, conn);
        conn->setThread(th);
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
        if (i == 0 && token != Message::Login) {
            // invalid login message
            conn->send(Message::Unauthorized);
            return nullptr;
        }
        if (i == 1) username = token;
        if (i == 2) password = token;
    }

    // this is where the password would actually be checked

    UserPtr user = getUser(username);
    if (user == nullptr) {
        user = std::make_shared<User>(*this, username);
        users.push_back(user);
    }

    conn->send(Message::Authorized);

    return user;
}

UserPtr VideoService::getUser(const string &username) const {
    auto it = std::find_if(users.begin(), users.end(), [&](const UserPtr &user) {
        return user->getUsername() == username;
    });
    return it != users.end() ? *it : nullptr;
}

const vector<ConnPtr>& VideoService::getConnections() const {
    return connections;
}

void VideoService::sendUserList(ConnPtr conn) {
    string msg = Message::List + " ";
    size_t len = users.size();
    for (int i = 0; i < len; i++) {
        msg += users[i]->getUsername();
        if (i < len - 1) msg += ',';
    }
    conn->send(msg);
}

void VideoService::disconnect(ConnPtr conn, bool close) {
    UserPtr user = conn->getUser();
    if (user != nullptr) {
        if (user->getConnections().size() == 1) {
            users.erase(std::remove(users.begin(), users.end(), user), users.end());
        }
    }

    conn->send(Message::Goodbye);
    conn->shutdown();
    if (close) {
        conn->close();
    }
}

int VideoService::getStatus() const {
    return status;
}

/// * Private methods * ///

void VideoService::handleConnection(ConnPtr conn) {
    UserPtr user = authenticate(conn);
    if (user == nullptr) {
        // authentication failed
        disconnect(conn, true);
        return;
    }

    conn->setUser(user);

    while (conn->getStatus() != Status::Shutdown) {
        string cmd;
        conn->recv(cmd);
        if (cmd == Message::List) {
            sendUserList(conn);
        } else if (cmd == Message::Disconnect) {
            disconnect(conn);
        }
    }

    conn->close();
    connections.erase(std::remove(connections.begin(), connections.end(), conn), connections.end());
}

int VideoService::init(int port, int backlog) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "failed to open socket\n");
        return Status::SocketErr;
    }
    if (::bind(socket_fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
        fprintf(stderr, "failed to bind socket\n");
        return Status::SocketErr;
    }
    if (listen(socket_fd, backlog) < 0) {
        fprintf(stderr, "failed to listen on socket\n");
        return Status::SocketErr;
    }
    printf("Accepting new connections...\n");
    return Status::Ok;
}
