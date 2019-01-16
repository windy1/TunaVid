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

class VideoClient;

using std::thread;
using std::unique_ptr;
using std::stringstream;
using std::vector;
using std::string;

class MessageListener {

    VideoClient &client;
    thread th;
    bool running;

    void _start();

    void readUserList(vector<string> &userList, stringstream &in) const;

public:

    explicit MessageListener(VideoClient &client);

    void start();

    void stop();

};


#endif //TUNAVID_MESSAGELISTENER_H
