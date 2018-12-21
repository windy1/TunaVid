//
// Created by Walker Crouse on 2018-12-18.
//

#ifndef TUNAVID_TUNA_H
#define TUNAVID_TUNA_H

///////////////////////////////////////
///                                 ///
///        == Status Codes ==       ///
///                                 ///
///////////////////////////////////////

#define STATUS_OK           200
#define STATUS_SHUTDOWN     201
#define STATUS_ERR_SOCKET   400
#define STATUS_INVALID_ARGS 401
#define STATUS_ERR_LOGIN    402

///////////////////////////////////////
///                                 ///
///     == Protocol messages ==     ///
///                                 ///
///////////////////////////////////////

#define MSG_LOGIN           "LOGIN"
#define MSG_UNAUTHORIZED    "UNAUTHORIZED"
#define MSG_AUTHORIZED      "AUTHORIZED"
#define MSG_LIST            "LIST"
#define MSG_OK              "OK"

#endif //TUNAVID_TUNA_H
