//
// Created by Walker Crouse on 2019-01-17.
//

#ifndef TUNAVID_SERVERMONITOR_H
#define TUNAVID_SERVERMONITOR_H

#include <thread>
#include <string>
#include <vector>
#include <map>
#include "ServerMonitorUi.h"
#include "../Connection.h"
#include "CallSession.h"

using std::thread;
using std::string;
using std::vector;
using std::map;

class VideoService;

class ServerMonitor {

    VideoService *service;
    ServerMonitorUi ui;
    bool is_running;
    thread watcher_thread;

    vector<ConnPtr> last_connections;
    map<string, ConnPtr> connections;

    vector<CallSessionPtr> last_call_sessions;
    map<string, CallSessionPtr> call_sessions;

    void startWatching();

public:

    ServerMonitor(VideoService *service);

    void start();

    VideoService* getService() const;

};


#endif //TUNAVID_SERVERMONITOR_H
