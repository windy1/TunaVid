//
// Created by Walker Crouse on 2018-12-18.
//

#include <dlfcn.h>
#include <sstream>
#include <vector>
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

VideoClient::VideoClient() : messageListener(MessageListener(*this)) {}

/// * Public methods * ///

int VideoClient::start(int argc, char *argv[]) {
    printf("[[starting client]]\n");
    if (argc != 4) {
        fprintf(stderr, "missing host and port arguments");
        return (status = Status::InvalidArgs);
    }
    host = argv[2];
    port = std::stoi(argv[3]);
    status = Status::Ok;

    ui.setLoginHandler(std::bind(&VideoClient::handleLogin, this, _1, _2));
    ui.start(argc, argv);

    shutdown();
    messageListener.stop();

    return status;
}

//bool VideoClient::refreshUserList() {
//    conn->send(Message::List);
//    string buffer;
//    conn->recv(buffer);
//
//    stringstream in(buffer);
//    string token;
//
//    std::getline(in, token, ' ');
//    if (token != Message::List) {
//        fprintf(stderr, "failed to refresh user list");
//        return false;
//    }
//
//    vector<string> userList;
//    while (std::getline(in, token, ',')) {
//        userList.push_back(token);
//    }
//    ui.setUserList(userList);
//
//    return true;
//}

ConnPtr VideoClient::getConnection() const {
    return conn;
}

Ui::TunaVid& VideoClient::getUi() const {
    return (Ui::TunaVid&) ui;
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

/// * Private methods * ///

void VideoClient::handleLogin(string username, string password) {
    conn = Connection::connect(host, port);
    if (conn == nullptr) return;
    messageListener.start();
    string msg = Message::Login + " " + username + " " + password;
    conn->send(msg);
//    string res;
//    conn->recv(res);
//
//    if (res == Message::Unauthorized) {
//        ui.postError("Unauthorized");
//        return;
//    } else if (res == Message::Authorized) {
//        ui.postError("Success");
//        conn->send(Message::List);
//        ui.showHome();
//    } else {
//        ui.postError("An unexpected error occurred.");
//    }
}

void VideoClient::shutdown() {
    printf("shutting down...\n");
    conn->send(Message::Disconnect);
//    string res;
//    conn->recv(res);
//    if (res == Message::Goodbye) {
//        conn->close();
//    } else {
//        fprintf(stderr, "failed to signoff gracefully");
//    }
//    status = Status::Closed;
}
