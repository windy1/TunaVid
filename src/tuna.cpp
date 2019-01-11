//
// Created by Walker Crouse on 2019-01-10.
//

#include "tuna.h"

///////////////////////////////////////
///                                 ///
///        == Status Codes ==       ///
///                                 ///
///////////////////////////////////////

const int Status::Ok            = 200;
const int Status::Shutdown      = 201;
const int Status::Closed        = 202;
const int Status::SocketErr     = 400;
const int Status::InvalidArgs   = 401;
const int Status::LoginErr      = 402;

///////////////////////////////////////
///                                 ///
///     == Protocol messages ==     ///
///                                 ///
///////////////////////////////////////

const string Message::Login         = "LOGIN";
const string Message::Unauthorized  = "UNAUTHORIZED";
const string Message::Authorized    = "AUTHORIZED";
const string Message::List          = "LIST";
const string Message::Disconnect    = "DISCONNECT";
const string Message::Goodbye       = "GOODBYE";
