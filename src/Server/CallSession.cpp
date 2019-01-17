//
// Created by Walker Crouse on 2019-01-11.
//

#include "CallSession.h"
#include "../tuna.h"
#include <pthread.h>
#include <fstream>

////////////////////////////////////////
///                                  ///
///         == CallSession ==        ///
///                                  ///
////////////////////////////////////////

int CallSession::LastId = -1;

CallSession::CallSession(ConnPtr senderConn, UserPtr receiver)
        : sender_conn(senderConn),
          sender(senderConn->getUser()),
          receiver(receiver),
          receiver_conn(nullptr),
          is_opened(false),
          id(++LastId) {
    th_main = thread(&CallSession::init, this);
}

/// * Public methods * ///

void CallSession::accepted(ConnPtr receiver_conn) {
    // both parties have accepted the call, and it can now be opened
    this->receiver_conn = receiver_conn;
    is_opened = true;
}

void CallSession::readFrame(ConnPtr conn) {
    string data;
    conn->recv(data);
    if (conn == sender_conn) {
        frame_buffer_r.push(data);
    } else if (conn == receiver_conn) {
        frame_buffer_s.push(data);
    } else {
        fprintf(stderr, "tried to read frame from connection not on the call\n");
    }
}

/// * Private methods * ///

void CallSession::init() {
    pthread_setname_np(("CallSession_main-" + std::to_string(id)).c_str());
    // notify receiver of incoming call on all active connections
    for (auto &conn : *receiver->getConnections()) {
        conn->send(Message::CallIncoming + " " + sender->getUsername() + " " + std::to_string(id));
    }
    while (!is_opened) continue;
    start();
}

void CallSession::start() {
    string message = Message::CallOpen + " " + std::to_string(id);
    sender_conn->send(message);
    receiver_conn->send(message);
    th_s = thread(&CallSession::bufferFrames, this, sender_conn, std::ref(frame_buffer_s));
    th_r = thread(&CallSession::bufferFrames, this, receiver_conn, std::ref(frame_buffer_r));
    th_s.join();
    th_r.join();
}

void CallSession::bufferFrames(ConnPtr out, queue<string> &buffer) {
    pthread_setname_np(("CallSession_bufferFrames-" + std::to_string(id)).c_str());

    std::ofstream file;
    int i = 0, j = 0;

    while (is_opened) {
        if (buffer.empty()) continue;
        string *data = &buffer.front();

//        if (++i >= 60) {
//            file = std::ofstream("server_" + out->getUser()->getUsername() + "_" + std::to_string(j) + ".jpeg");
//            file << *data;
//            file.close();
//            i = 0;
//            j++;
//            fprintf(stderr, "wrote image to disk\n");
//        }

        out->sendMulti({Message::Frame + " " + std::to_string(id), *data});
        buffer.pop();
    }
}

/// * Getters * ///

UserPtr CallSession::getSender() const {
    return sender;
}

UserPtr CallSession::getReceiver() const {
    return receiver;
}

ConnPtr CallSession::getSenderConn() const {
    return sender_conn;
}

ConnPtr CallSession::getReceiverConn() const {
    return receiver_conn;
}

bool CallSession::isOpened() const {
    return is_opened;
}

int CallSession::getId() const {
    return id;
}
