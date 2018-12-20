//
// Created by Walker Crouse on 2018-12-18.
//

#include <dlfcn.h>
#include "VideoClient.h"
#include "../tuna.h"
#include "../Connection.h"

using std::function;
using namespace std::placeholders;

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

int VideoClient::getStatus() const {
    return status;
}

void VideoClient::handleLogin(string username, string password) {
    printf("username = %s\n", username.c_str());
    printf("password = %s\n", password.c_str());
    ui.postError("Test");
    ConnPtr conn = Connection::connect(host, port);
    if (conn == nullptr) return;
    conn->send("LOGIN " + username + " " + password);
}
