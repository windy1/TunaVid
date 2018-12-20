//
// Created by Walker Crouse on 2018-12-18.
//

#ifndef TUNAVID_VIDEOSERVICE_H
#define TUNAVID_VIDEOSERVICE_H

#include "../tuna.h"
#include "../Connection.h"
#include <vector>

using std::vector;

class VideoService {

    int status;
    int socket_fd;
    int running;
    vector<ConnPtr> connections;

    int init(int port, int backlog);

    void handleConnection(ConnPtr conn);

public:

    int start(int port, int backlog=10);

    int getStatus() const;

};

#endif //TUNAVID_VIDEOSERVICE_H
