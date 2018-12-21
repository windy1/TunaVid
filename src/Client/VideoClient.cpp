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

/// * Public methods * ///

int VideoClient::start(int argc, char *argv[]) {
    printf("[[starting client]]\n");
    if (argc != 4) {
        fprintf(stderr, "missing host and port arguments");
        return (status = STATUS_INVALID_ARGS);
    }
    host = argv[2];
    port = std::stoi(argv[3]);
    status = STATUS_OK;
    ui.setLoginHandler(std::bind(&VideoClient::handleLogin, this, _1, _2));
    ui.start(argc, argv);
    return status;
}

bool VideoClient::refreshUserList() {
    conn->send(MSG_LIST);
    string buffer;
    conn->recv(buffer);

    stringstream in(buffer);
    string token;

    std::getline(in, token, ' ');
    printf("buffer = %s\n", buffer.c_str());
    if (token != MSG_LIST) {
        fprintf(stderr, "failed to refresh user list");
        return false;
    }

    vector<string> userList;
    while (std::getline(in, token, ',')) {
        userList.push_back(token);
    }
    ui.setUserList(userList);

    return true;
}

int VideoClient::getStatus() const {
    return status;
}

/// * Private methods * ///

void VideoClient::handleLogin(string username, string password) {
    printf("username = %s\n", username.c_str());
    printf("password = %s\n", password.c_str());
    conn = Connection::connect(host, port);
    if (conn == nullptr) return;
    char msg[1024];
    sprintf(msg, "%s %s %s", MSG_LOGIN, username.c_str(), password.c_str());
    conn->send(msg);
    string res;
    conn->recv(res);
    if (res == MSG_UNAUTHORIZED) {
        ui.postError("Unauthorized");
        return;
    } else if (res == MSG_AUTHORIZED) {
        ui.postError("Success");
        refreshUserList();
        ui.showHome();
    } else {
        ui.postError("An unexpected error occurred.");
    }
}
