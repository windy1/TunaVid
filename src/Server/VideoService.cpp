//
// Created by Walker Crouse on 2018-12-18.
//

#include "VideoService.h"
#include "../Connection.h"
#include <unistd.h>
#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <future>
#include <pthread.h>

using namespace std::placeholders;

////////////////////////////////////////
///                                  ///
///        == VideoService ==        ///
///                                  ///
////////////////////////////////////////

VideoService::VideoService() : monitor(ServerMonitor(this)) {
    initHandlers();
}

/// * Public methods * ///

int VideoService::start(int port, int backlog) {
    pthread_setname_np("Main");

    printf("[[starting service]]\n");
    printf("port = %d\n", port);

    if ((status = init(port, backlog)) != Status::Ok) {
        return status;
    }

    listen_thread = thread(&VideoService::startListening, this);
    monitor.start();
    listen_thread.join();

    return status;
}

UserPtr VideoService::authenticate(ConnPtr conn) {
    assert(conn != nullptr);
    assert(conn->getUser() == nullptr);

    string login;
    if (!conn->recv(login)) return authenticate(conn);

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

    // TODO: this is where the password would actually be checked

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
    assert(conn != nullptr);
    string msg = Message::List + " ";
    size_t len = users.size();
    for (int i = 0; i < len; i++) {
        msg += users[i]->getUsername();
        if (i < len - 1) msg += ',';
    }
    conn->send(msg);
}

void VideoService::disconnect(ConnPtr conn, bool close) {
    assert(conn != nullptr);
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
    assert(!message.empty());
    std::async(&VideoService::_sendAll, this, message);
}

/// * Private methods * ///

int VideoService::init(int port, int backlog) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("failed to open socket");
        return Status::SocketErr;
    }
    if (bind(socket_fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("failed to bind socket");
        return Status::SocketErr;
    }
    if (listen(socket_fd, backlog) < 0) {
        perror("failed to listen on socket");
        return Status::SocketErr;
    }
    printf("Accepting new connections...\n");
    return Status::Ok;
}

void VideoService::initHandlers() {
    handlerMap = {
        {Message::List, std::bind(&VideoService::onList, this, _1, _2)},
        {Message::Call, std::bind(&VideoService::onCall, this, _1, _2)},
        {Message::CallAccept, std::bind(&VideoService::onCallAccept, this, _1, _2)},
        {Message::Frame, std::bind(&VideoService::onFrame, this, _1, _2)},
        {Message::Disconnect, std::bind(&VideoService::onDisconnect, this, _1, _2)}
    };
}

void VideoService::startListening() {
    sockaddr_in cli_addr{};
    int addr_len = sizeof(cli_addr);
    int cli_socket;

    // start accepting new connections
    is_running = true;
    while (is_running) {
        cli_socket = accept(socket_fd, (sockaddr *) &cli_addr, (socklen_t *) &addr_len);
        if (cli_socket < 0) {
            continue;
        }
        ConnPtr conn = std::make_shared<Connection>(cli_socket);
        connections.push_back(conn);
        shared_ptr<thread> th = std::make_shared<thread>(&VideoService::handleConnection, this, conn);
        conn->setThread(th);
    }

    printf("Shutting down...\n");

    for (auto &call : call_sessions) call->close();
    for (auto &call : call_sessions) call->getMainThread()->join();
    call_sessions.clear();

    for (auto &conn : connections) disconnect(conn);
    for (auto &conn : connections) conn->getThread()->join();
    connections.clear();

    users.clear();

    close(socket_fd);

    printf("Goodbye.\n");
}

void VideoService::handleConnection(ConnPtr conn) {
    assert(conn != nullptr);

    // authenticate the connection
    UserPtr user = authenticate(conn);
    if (user == nullptr) {
        // authentication failed
        disconnect(conn, true);
        return;
    }
    conn->setUser(user);

    pthread_setname_np(("Connection-" + user->getUsername()).c_str());

    int callId = -1;
    string message;
    stringstream in;
    string header;

    while (conn->getStatus() != Status::Shutdown) {
        if (!conn->recv(message)) continue;
        if (conn->getStatus() == Status::Shutdown) break;
        in = stringstream(message);
        in >> header;
        auto handler = handlerMap.find(header);
        if (handler == handlerMap.end()) {
            fprintf(stderr, "no handler for header %s\n", header.c_str());
        } else {
            handler->second(conn, in);
        }
    }

    conn->close();
    connections.erase(std::remove(connections.begin(), connections.end(), conn), connections.end());
}

void VideoService::_sendAll(const string &message) {
    for (auto &conn : connections) {
        if (conn->getUser() != nullptr) {
            conn->send(message);
        }
    }
}

void VideoService::onList(ConnPtr conn, stringstream &in) {
    sendUserList(conn);
}

void VideoService::onCall(ConnPtr conn, stringstream &in) {
    string receiver_name;
    in >> receiver_name;
    UserPtr receiver = getUser(receiver_name);
    if (receiver == nullptr) {
        conn->send(Message::CallInvalid);
    } else {
        CallSessionPtr call = std::make_shared<CallSession>(conn, receiver);
        call_sessions.push_back(call);
        conn->send(Message::CallWaiting + " " + std::to_string(call->getId()));
    }
}

void VideoService::onCallAccept(ConnPtr conn, stringstream &in) {
    int callId;
    in >> callId;
    CallSessionPtr call = getCall(callId);
    if (call == nullptr || call->getReceiver() != conn->getUser() || call->isOpened()) {
        conn->send(Message::CallInvalid);
        return;
    }
    call->accepted(conn);
}

void VideoService::onCallIgnore(ConnPtr conn, stringstream &in) {
    int callId;
    in >> callId;
    CallSessionPtr call = getCall(callId);
    if (call == nullptr || call->getReceiver() != conn->getUser() || call->isOpened()) {
        conn->send(Message::CallInvalid);
        return;
    }
    call->ignored();
}

void VideoService::onFrame(ConnPtr conn, stringstream &in) {
    int callId;
    in >> callId;
    CallSessionPtr call = getCall(callId);
    if (call == nullptr) {
        conn->send(Message::CallInvalid);
        return;
    }
    call->readFrame(conn);
}

void VideoService::onDisconnect(ConnPtr conn, stringstream &in) {
    disconnect(conn);
}

/// * Getters * ///

const vector<ConnPtr>& VideoService::getConnections() const {
    return connections;
}

const vector<CallSessionPtr>& VideoService::getCallSessions() const {
    return call_sessions;
}

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

bool VideoService::isRunning() const {
    return is_running;
}

int VideoService::getStatus() const {
    return status;
}
