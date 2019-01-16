//
// Created by Walker Crouse on 2019-01-11.
//

#include "MessageListener.h"
#include "VideoClient.h"
#include "../tuna.h"
#include <pthread.h>
#include <fstream>

MessageListener::MessageListener(VideoClient &client): client(client) {}

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
    Ui::TunaVid &ui = client.getUi();
    UserPtr user;
    bool frameIncoming = false;
    string message;
    stringstream in;
    string header;

    std::ofstream out;
    int i = 0, j = 0;

    while (running) {
        if (!running) break;

        if (frameIncoming) {
            string imageData;
            conn->recv(imageData);

            if (++i >= 60) {
                string fname = "client_" + user->getUsername() + "_" + std::to_string(j) + ".jpeg";
                out = std::ofstream(fname);
                out << imageData;
                out.close();
                j++;
                i = 0;
                fprintf(stderr, "wrote image to disk: %s\n", fname.c_str());
            }

            ui.receiveFrame(imageData);
            frameIncoming = false;
            continue;
        }

        conn->recv(message);
        in = stringstream(message);
        in >> header;

        if (header == Message::Authorized) {
            conn->send(Message::List);
            ui.showHome();
            user = client.getUser();
        } else if (header == Message::Unauthorized) {
            ui.postError("Unauthorized");
            client.setUser(nullptr);
        } else if (header == Message::List) {
            vector<string> userList;
            readUserList(userList, in);
            ui.setUserList(userList);
        } else if (header == Message::Join) {
            string username;
            in >> username;
            ui.addUser(username);
        } else if (header == Message::Leave) {
            string username;
            in >> username;
            ui.removeUser(username);
        } else if (header == Message::Frame) {
            frameIncoming = true;
        } else if (header == Message::CallWaiting) {
            int callId;
            in >> callId;
            client.setActiveCallId(callId);
        } else if (header == Message::CallIncoming) {
            string sender;
            int callId;
            in >> sender;
            in >> callId;
            ui.showCallAlert(sender);
            client.setPendingCallId(callId);
        } else if (header == Message::CallOpen) {
            int callId;
            in >> callId;
            client.setActiveCallId(callId);
            client.setPendingCallId(-1);
            ui.startWritingFrames();
        } else if (header == Message::CallInvalid) {
            // TODO
        } else if (header == Message::Goodbye) {
            conn->close();
            running = false;
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
