//
// Created by Walker Crouse on 2018-12-18.
//

#include <dlfcn.h>
#include <sstream>
#include <vector>
#include <fstream>
#include "VideoClient.h"
#include "../tuna.h"

using std::function;
using std::stringstream;
using std::vector;
using namespace std::placeholders;

////////////////////////////////////////
///                                  ///
///        == Video Client ==        ///
///                                  ///
////////////////////////////////////////

VideoClient::VideoClient() : message_listener(MessageListener(*this)) {}

/// * Public methods * ///

int VideoClient::start(int argc, char *argv[]) {
    pthread_setname_np("Main");

    printf("[[starting client]]\n");
    if (argc != 4) {
        fprintf(stderr, "missing host and port arguments");
        return (status = Status::InvalidArgs);
    }
    host = argv[2];
    port = std::stoi(argv[3]);
    status = Status::Ok;

    ui.setLoginHandler(std::bind(&VideoClient::handleLogin, this, _1, _2));
    ui.setCallHandler(std::bind(&VideoClient::handleCall, this, _1, _2));
    ui.setCaptureHandler(std::bind(&VideoClient::handleFrame, this, _1));
    ui.start(argc, argv);

    shutdown();
    message_listener.stop();

    return status;
}

/// * Private methods * ///

void VideoClient::handleLogin(const string &username, const string &password) {
    conn = Connection::connect(host, port);
    if (conn == nullptr) return;
    message_listener.start();
    user = std::make_shared<User>(nullptr, username);
    conn->send(Message::Login + " " + username + " " + password);
}

void VideoClient::handleCall(const string &receiver, CallAction action) {
    switch (action) {
        case Start:
            conn->send(Message::Call + " " + receiver);
            break;
        case Accept:
            conn->send(Message::CallAccept + " " + std::to_string(pendingCallId));
            break;
        default:
            printf("unknown call action %d\n", action);
            break;
    }
}

void VideoClient::handleFrame(const string &data) {
    conn->sendMulti({Message::Frame + " " + std::to_string(activeCallId), data});
}

void VideoClient::shutdown() {
    printf("shutting down...\n");
    conn->send(Message::Disconnect);
}

/// * Setters * ///

void VideoClient::setUser(UserPtr user) {
    this->user = user;
}

void VideoClient::setActiveCallId(int callId) {
    this->activeCallId = callId;
}

void VideoClient::setPendingCallId(int callId) {
    this->pendingCallId = callId;
}

/// * Getters * ///

ConnPtr VideoClient::getConnection() const {
    return conn;
}

Ui::TunaVid& VideoClient::getUi() {
    return ui;
}

UserPtr VideoClient::getUser() const {
    return user;
}

int VideoClient::getActiveCallId() const {
    return activeCallId;
}

int VideoClient::getPendingCallId() const {
    return pendingCallId;
}

const string& VideoClient::getHost() const {
    return host;
}

int VideoClient::getPort() const {
    return port;
}

int VideoClient::getStatus() const {
    return status;
}
