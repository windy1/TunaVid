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
          is_ignored(false),
          id(++LastId) {
    th_main = thread(&CallSession::init, this);
}

/// * Public methods * ///

void CallSession::accepted(ConnPtr receiver_conn) {
    assert(receiver_conn != nullptr);
    assert(receiver_conn->getUser() != nullptr);
    assert(receiver_conn->getUser() == receiver);
    assert(!is_opened);
    // both parties have accepted the call, and it can now be opened
    this->receiver_conn = receiver_conn;
    is_opened = true;
}

void CallSession::ignored() {
    is_ignored = true;
}

void CallSession::readFrame(ConnPtr conn) {
    assert(conn != nullptr);

    string data;
    if (!conn->recv(data)) return;

    if (conn->getStatus() != Status::Ok) {
        is_opened = false;
        return;
    }

    if (conn == sender_conn) {
        frame_buffer_r.push(data);
    } else if (conn == receiver_conn) {
        frame_buffer_s.push(data);
    } else {
        fprintf(stderr, "tried to read frame from connection not on the call\n");
    }
}

void CallSession::close() {
    if (!is_opened) return;
    is_opened = false;
    string message = Message::CallClose + " " + std::to_string(id);
    sender_conn->send(message);
    receiver_conn->send(message);
}

/// * Private methods * ///

void CallSession::init() {
    pthread_setname_np(("CallSession_main-" + std::to_string(id)).c_str());
    // notify receiver of incoming call on all active connections
    for (auto &conn : *receiver->getConnections()) {
        conn->send(Message::CallIncoming + " " + sender->getUsername() + " " + std::to_string(id));
    }

    while (!is_opened && !is_ignored);

    if (is_ignored) {
        sender_conn->send(Message::CallIgnore + " " + std::to_string(id));
    } else {
        start();
    }
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
    while (is_opened) {
        if (buffer.empty()) continue;
        string *data = &buffer.front();
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

thread* CallSession::getMainThread() {
    return &th_main;
}

bool CallSession::isOpened() const {
    return is_opened;
}

bool CallSession::isIgnored() const {
    return is_ignored;
}

int CallSession::getId() const {
    return id;
}
