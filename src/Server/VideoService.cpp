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
#include <future>
#include <pthread.h>

using std::thread;
using std::stringstream;

////////////////////////////////////////
///                                  ///
///        == VideoService ==        ///
///                                  ///
////////////////////////////////////////

/// * Public methods * ///

int VideoService::start(int port, int backlog) {
    pthread_setname_np("Main");

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
        th->detach();
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
        // user is logging on
        user = std::make_shared<User>(this, username);
        users.push_back(user);
        sendAll(Message::Join + " " + username);
    }

    conn->send(Message::Authorized);

    return user;
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
    if (user != nullptr && user->getConnections()->size() == 1) {
        // user logging off
        users.erase(std::remove(users.begin(), users.end(), user), users.end());
        sendAll(Message::Leave + " " + user->getUsername());
    }

    conn->send(Message::Goodbye);
    conn->shutdown();
    if (close) {
        conn->close();
    }
}

void VideoService::sendAll(const string &message) {
    std::async(&VideoService::_sendAll, this, message);
}

void VideoService::startCall(ConnPtr sender, UserPtr receiver) {
    if (receiver == nullptr) {
        sender->send(Message::CallInvalid);
    } else {
        CallSessionPtr call = std::make_shared<CallSession>(sender, receiver);
        call_sessions.push_back(call);
        sender->send(Message::CallWaiting + " " + std::to_string(call->getId()));
    }
}

void VideoService::acceptCall(int callId, ConnPtr conn) {
    CallSessionPtr call = getCall(callId);
    if (call == nullptr || call->getReceiver() != conn->getUser()) {
        conn->send(Message::CallInvalid);
        return;
    }
    call->accepted(conn);
}

/// * Private methods * ///

void VideoService::handleConnection(ConnPtr conn) {
    UserPtr user = authenticate(conn);
    if (user == nullptr) {
        // authentication failed
        disconnect(conn, true);
        return;
    }

    pthread_setname_np(("Connection-" + user->getUsername()).c_str());

    conn->setUser(user);
    int callId = -1;
    bool frameIncoming = false;
    string message;
    stringstream in;
    string header;

    while (conn->getStatus() != Status::Shutdown) {
        if (frameIncoming) {
            handleFrame(conn, callId);
            frameIncoming = false;
            continue;
        }

        conn->recv(message);
        in = stringstream(message);
        in >> header;

        if (header == Message::List) {
            sendUserList(conn);
        } else if (header == Message::Call) {
            string receiver_name;
            in >> receiver_name;
            UserPtr receiver = getUser(receiver_name);
            startCall(conn, receiver);
        } else if (header == Message::CallAccept) {
            in >> callId;
            acceptCall(callId, conn);
        } else if (header == Message::Frame) {
            in >> callId;
            frameIncoming = true;
        } else if (header == Message::Disconnect) {
            disconnect(conn);
        }
    }

    conn->close();
    connections.erase(std::remove(connections.begin(), connections.end(), conn), connections.end());
}

void VideoService::handleFrame(ConnPtr sender, int callId) {
    CallSessionPtr call = getCall(callId);
    if (call == nullptr) {
        sender->send(Message::CallInvalid);
        return;
    }
    call->readFrame(sender);
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

void VideoService::_sendAll(const string &message) {
    for (auto &conn : connections) {
        conn->send(message);
    }
}

/// * Getters * ///

UserPtr VideoService::getUser(const string &username) const {
    auto it = std::find_if(users.begin(), users.end(), [&](const UserPtr &user) {
        return user->getUsername() == username;
    });
    return it != users.end() ? *it : nullptr;
}

CallSessionPtr VideoService::getCall(int callId) const {
    auto it = std::find_if(call_sessions.begin(), call_sessions.end(), [&](const CallSessionPtr &call) {
        return call->getId() == callId;
    });
    return it != call_sessions.end() ? *it : nullptr;
}

const vector<ConnPtr>& VideoService::getConnections() const {
    return connections;
}

int VideoService::getStatus() const {
    return status;
}
