//
// Created by Walker Crouse on 2019-01-11.
//

#ifndef TUNAVID_CALLSESSION_H
#define TUNAVID_CALLSESSION_H

#include "../User.h"
#include <thread>
#include <memory>
#include <queue>
#include <mutex>

using std::thread;
using std::shared_ptr;
using std::queue;
using std::mutex;

class CallSession;

typedef shared_ptr<CallSession> CallSessionPtr;

class CallSession {

    static int LastId;

    UserPtr sender;
    UserPtr receiver;
    ConnPtr sender_conn;
    ConnPtr receiver_conn;
    bool is_opened;
    int id;

    thread th_main;
    thread th_s;
    thread th_r;

    mutex mutex_s;
    mutex mutex_r;

    queue<string> frame_buffer_s;
    queue<string> frame_buffer_r;

    void init();

    void start();

    void bufferFrames(ConnPtr out, queue<string> &buffer, mutex &mut);

public:

    CallSession(ConnPtr senderConn, UserPtr receiver);

    UserPtr getSender() const;

    UserPtr getReceiver() const;

    ConnPtr getSenderConn() const;

    ConnPtr getReceiverConn() const;

    bool isOpened() const;

    int getId() const;

    void accepted(ConnPtr receiver_conn);

    void readFrame(ConnPtr conn);

};

#endif //TUNAVID_CALLSESSION_H
