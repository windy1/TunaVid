//
// Created by Walker Crouse on 2019-01-11.
//

#ifndef TUNAVID_MESSAGELISTENER_H
#define TUNAVID_MESSAGELISTENER_H

#include <memory>
#include <thread>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <functional>

class VideoClient;

using std::thread;
using std::unique_ptr;
using std::stringstream;
using std::vector;
using std::string;
using std::map;
using std::function;

class MessageListener {

    VideoClient &client;
    thread th;
    bool running;
    map<string, function<void(const string&, stringstream&)>> handlerMap;

    int i, j;

    void _start();

    void readUserList(vector<string> &userList, stringstream &in) const;

    void onAuthorized(const string &header, stringstream &in);
    void onUnauthorized(const string &header, stringstream &in);
    void onList(const string &header, stringstream &in);
    void onJoin(const string &header, stringstream &in);
    void onLeave(const string &header, stringstream &in);
    void onFrame(const string &header, stringstream &in);
    void onCallWaiting(const string &header, stringstream &in);
    void onCallIncoming(const string &header, stringstream &in);
    void onCallOpen(const string &header, stringstream &in);
    void onCallInvalid(const string &header, stringstream &in);
    void onGoodbye(const string &header, stringstream &in);

public:

    explicit MessageListener(VideoClient &client);

    void start();

    void stop();

};


#endif //TUNAVID_MESSAGELISTENER_H
