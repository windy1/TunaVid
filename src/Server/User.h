//
// Created by Walker Crouse on 2018-12-20.
//

#ifndef TUNAVID_USER_H
#define TUNAVID_USER_H

#include <string>
#include <memory>

using std::string;

class User;

typedef std::shared_ptr<User> UserPtr;

/**
 * Represents a User of the VideoService.
 */
class User {

    string username;

public:

    explicit User(const string &username);

    /**
     * Returns this user's username.
     *
     * @return user's username
     */
    const string& getUsername() const;

};

#endif //TUNAVID_USER_H
