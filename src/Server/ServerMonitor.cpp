//
// Created by Walker Crouse on 2019-01-17.
//

#include <unistd.h>
#include "ServerMonitor.h"
#include "VideoService.h"

ServerMonitor::ServerMonitor(VideoService *service) : service(service) {}

void ServerMonitor::start() {
    printf("[[starting server monitor]]\n");
    watcher_thread = thread(&ServerMonitor::startWatching, this);
    is_running = true;
    ui.start();
    is_running = false;
}

void ServerMonitor::startWatching() {
    while (is_running) {
        const vector<ConnPtr> &connections = service->getConnections();
        if (connections != last_connections) {
            last_connections = connections;
            this->connections.clear();

            for (auto &conn : last_connections) {
                string name = "unknown";
                UserPtr user = conn->getUser();
                if (user != nullptr) {
                    name = user->getUsername();
                }
                string str = name + " (" + std::to_string(conn->getId()) + ")";
                this->connections[str] = conn;
            }

            vector<string> keys;
            for (auto &entry : this->connections) {
                keys.push_back(entry.first);
            }
            ui.updateConnections(keys);
        }

        const vector<CallSessionPtr> &calls = service->getCallSessions();
        if (calls != last_call_sessions) {
            last_call_sessions = calls;
            call_sessions.clear();
            for (auto &call : last_call_sessions) {
                string title = call->getSender()->getUsername() + " => " + call->getReceiver()->getUsername();
                call_sessions[title] = call;
            }

            vector<string> keys;
            for (auto &entry : call_sessions) {
                keys.push_back(entry.first);
            }
        }
    }
}

VideoService* ServerMonitor::getService() const {
    return service;
}
