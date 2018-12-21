//
// Created by Walker Crouse on 2018-12-20.
//

#include "User.h"

////////////////////////////////////////
///                                  ///
///            == User ==            ///
///                                  ///
////////////////////////////////////////

User::User(const string &username) : username(username) {}

/// * Public methods * ///

const string& User::getUsername() const {
    return username;
}
