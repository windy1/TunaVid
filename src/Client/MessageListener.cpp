//
// Created by Walker Crouse on 2019-01-11.
//

#include "MessageListener.h"
#include "VideoClient.h"
#include "../tuna.h"
#include <pthread.h>
#include <fstream>

using namespace std::placeholders;

MessageListener::MessageListener(VideoClient &client): client(client), i(0), j(0) {
    handlerMap = {
        {Message::Authorized, std::bind(&MessageListener::onAuthorized, this, _1, _2)},
        {Message::Unauthorized, std::bind(&MessageListener::onUnauthorized, this, _1, _2)},
        {Message::List, std::bind(&MessageListener::onList, this, _1, _2)},
        {Message::Join, std::bind(&MessageListener::onJoin, this, _1, _2)},
        {Message::Leave, std::bind(&MessageListener::onLeave, this, _1, _2)},
        {Message::Frame, std::bind(&MessageListener::onFrame, this, _1, _2)},
        {Message::CallWaiting, std::bind(&MessageListener::onCallWaiting, this, _1, _2)},
        {Message::CallIncoming, std::bind(&MessageListener::onCallIncoming, this, _1, _2)},
        {Message::CallOpen, std::bind(&MessageListener::onCallOpen, this, _1, _2)},
        {Message::CallInvalid, std::bind(&MessageListener::onCallInvalid, this, _1, _2)},
        {Message::Goodbye, std::bind(&MessageListener::onGoodbye, this, _1, _2)}
    };
}

void MessageListener::start() {
    running = true;
    th = thread(&MessageListener::_start, this);
}

void MessageListener::stop() {
    running = false;
    th.join();
}

void MessageListener::_start() {
    pthread_setname_np("MessageListener");

    ConnPtr conn = client.getConnection();
    string message;
    string header;
    stringstream in;

    while (running) {
        conn->recv(message);
        in = stringstream(message);
        in >> header;
        auto it = handlerMap.find(header);
        if (it != handlerMap.end()) {
            it->second(header, in);
        } else {
            fprintf(stderr, "unknown header: %s\n", header.c_str());
        }
    }
}

void MessageListener::readUserList(vector<string> &userList, stringstream &in) const {
    string list;
    in >> list;
    stringstream list_in(list);
    string username;
    while (std::getline(list_in, username, ',')) {
        userList.push_back(username);
    }
}

void MessageListener::onAuthorized(const string &header, stringstream &in) {
    client.getConnection()->send(Message::List);
    client.getUi().showHome();
}

void MessageListener::onUnauthorized(const string &header, stringstream &in) {
    client.getUi().postError("Unauthorized");
    client.setUser(nullptr);
}

void MessageListener::onList(const string &header, stringstream &in) {
    vector<string> userList;
    readUserList(userList, in);
    client.getUi().setUserList(userList);
}

void MessageListener::onJoin(const string &header, stringstream &in) {
    string username;
    in >> username;
    client.getUi().addUser(username);
}

void MessageListener::onLeave(const string &header, stringstream &in) {
    string username;
    in >> username;
    client.getUi().removeUser(username);
}

void MessageListener::onFrame(const string &header, stringstream &in) {
    string imageData;
    client.getConnection()->recv(imageData);

//    if (++i >= 60) {
//        string fname = "client_" + client.getUser()->getUsername() + "_" + std::to_string(j) + ".jpeg";
//        std::ofstream out(fname);
//        out << imageData;
//        out.close();
//        j++;
//        i = 0;
//        fprintf(stderr, "wrote image to disk: %s\n", fname.c_str());
//    }

    client.getUi().receiveFrame(imageData);
}

void MessageListener::onCallWaiting(const string &header, stringstream &in) {
    int callId;
    in >> callId;
    client.setActiveCallId(callId);
}

void MessageListener::onCallIncoming(const string &header, stringstream &in) {
    string sender;
    int callId;
    in >> sender;
    in >> callId;
    client.getUi().showCallAlert(sender);
    client.setPendingCallId(callId);
}

void MessageListener::onCallOpen(const string &header, stringstream &in) {
    int callId;
    in >> callId;
    client.setActiveCallId(callId);
    client.setPendingCallId(-1);
    client.getUi().startWritingFrames();
}

void MessageListener::onCallInvalid(const string &header, stringstream &in) {
    // TODO
}

void MessageListener::onGoodbye(const string &header, stringstream &in) {
    client.getConnection()->close();
    running = false;
}
