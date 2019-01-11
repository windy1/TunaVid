//
// Created by Walker Crouse on 2018-12-20.
//

#ifndef TUNAVID_USER_H
#define TUNAVID_USER_H

#include <string>
#include <memory>
#include <vector>
#include "../Connection.h"

using std::vector;
using std::string;

class User;
class VideoService;

typedef std::shared_ptr<User> UserPtr;

/**
 * Represents a User of the VideoService.
 */
class User {

    VideoService &service;
    string username;
    mutable vector<ConnPtr> connections;

public:

    explicit User(VideoService &service, const string &username);

    /**
     * Returns this user's username.
     *
     * @return user's username
     */
    const string& getUsername() const;

    /**
     * Returns all Connections that have been authenticated with this user.
     *
     * @return connections of user
     */
    const vector<ConnPtr>& getConnections() const;

};

#endif //TUNAVID_USER_H
