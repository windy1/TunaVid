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
const string Message::Join          = "JOIN";
const string Message::Leave         = "LEAVE";
const string Message::Frame         = "FRAME";
const string Message::Call          = "CALL";
const string Message::CallWaiting   = "CALL_WAITING";
const string Message::CallInvalid   = "CALL_INVALID";
const string Message::CallIncoming  = "CALL_INCOMING";
const string Message::CallAccept    = "CALL_ACCEPT";
const string Message::CallOpen      = "CALL_OPEN";
const string Message::CallReject    = "CALL_REJECT";
const string Message::Goodbye       = "GOODBYE";
