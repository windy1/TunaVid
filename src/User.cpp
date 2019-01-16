//
// Created by Walker Crouse on 2018-12-20.
//

#include "User.h"
#include "Server/VideoService.h"

////////////////////////////////////////
///                                  ///
///            == User ==            ///
///                                  ///
////////////////////////////////////////

User::User(VideoService *service, const string &username) : service(service), username(username) {}

/// * Public methods * ///

const string& User::getUsername() const {
    return username;
}

const vector<ConnPtr>* User::getConnections() const {
    if (service == nullptr) {
        fprintf(stderr, "cannot get active connections while in client mode\n");
        return nullptr;
    }
    connections.clear();
    for (auto &conn : service->getConnections()) {
        if (conn->getUser().get() == this) {
            connections.push_back(conn);
        }
    }
    return &connections;
}
