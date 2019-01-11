//
// Created by Walker Crouse on 2019-01-11.
//

#include "MessageListener.h"
#include "VideoClient.h"
#include "../tuna.h"
#include <sstream>
#include <vector>
#include <string>

using std::string;
using std::stringstream;
using std::vector;

MessageListener::MessageListener(VideoClient &client): client(client) {}

void MessageListener::start() {
    running = true;
    th = std::make_unique<thread>(std::bind(&MessageListener::_start, this));
}

void MessageListener::stop() {
    running = false;
    th->join();
}

void MessageListener::_start() {
    ConnPtr conn = client.getConnection();
    Ui::TunaVid &ui = client.getUi();
    while (running) {
        string message;
        conn->recv(message);

        stringstream in(message);
        string header;
        in >> header;

        if (header == Message::Authorized) {
            conn->send(Message::List);
            ui.showHome();
        } else if (header == Message::Unauthorized) {
            ui.postError("Unauthorized");
        } else if (header == Message::List) {
            vector<string> userList;
            readUserList(userList, in);
            ui.setUserList(userList);
        } else if (header == Message::Goodbye) {
            conn->close();
            running = false;
        }
    }
}

void MessageListener::readUserList(vector<string> &userList, stringstream &in) const {
    string username;
    while (in >> username) {
        userList.push_back(username);
    }
}
