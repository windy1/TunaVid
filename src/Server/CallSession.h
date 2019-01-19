//
// Created by Walker Crouse on 2019-01-11.
//

#ifndef TUNAVID_CALLSESSION_H
#define TUNAVID_CALLSESSION_H

#include "../User.h"
#include <thread>
#include <memory>
#include <queue>

using std::thread;
using std::shared_ptr;
using std::queue;

class CallSession;

typedef shared_ptr<CallSession> CallSessionPtr;

/**
 * Represents a video-call made between one User to another.
 */
class CallSession {

    static int LastId;

    bool is_opened;
    bool is_ignored;
    int id;
    UserPtr sender;
    UserPtr receiver;
    ConnPtr sender_conn;
    ConnPtr receiver_conn;

    thread th_main;
    thread th_s;
    thread th_r;

    queue<string> frame_buffer_s;
    queue<string> frame_buffer_r;

    void init();

    void start();

    void bufferFrames(ConnPtr out, queue<string> &buffer);

public:

    CallSession(ConnPtr senderConn, UserPtr receiver);

    /**
     * Marks the call as accepted by the specified connection and opens the
     * call for frame transmission.
     *
     * @param receiver_conn connection of receiver accepting the call
     */
    void accepted(ConnPtr receiver_conn);

    void ignored();

    /**
     * Reads in a new frame from the specified connection and transmits it to
     * the other party.
     *
     * @param conn connection to read frame from
     */
    void readFrame(ConnPtr conn);

    /**
     * Closes the call.
     */
    void close();

    /**
     * Returns the User who initiated this call.
     *
     * @return User who initiated the call
     */
    UserPtr getSender() const;

    /**
     * Returns the User who is receiving the call.
     *
     * @return User who is receiving the call
     */
    UserPtr getReceiver() const;

    /**
     * Returns the senders connection.
     *
     * @return sender connection
     */
    ConnPtr getSenderConn() const;

    /**
     * Returns the receivers connection.
     *
     * @return receiver connection
     */
    ConnPtr getReceiverConn() const;

    /**
     * Returns the main thread for this call.
     *
     * @return main thread
     */
    thread* getMainThread();

    /**
     * Returns true if this call is opened and is actively receiving /
     * transmitting frames.
     *
     * @return true if call is opened
     */
    bool isOpened() const;

    /**
     * Returns true if this call was ignored by the receiver.
     *
     * @return true if call was ignored
     */
    bool isIgnored() const;

    /**
     * Returns this calls unique ID.
     *
     * @return unique ID
     */
    int getId() const;

};

#endif //TUNAVID_CALLSESSION_H
