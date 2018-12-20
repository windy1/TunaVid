//
// Created by Walker Crouse on 2018-12-18.
//

#include "VideoService.h"
#include "../Connection.h"
#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <thread>

using std::thread;

int VideoService::start(int port, int backlog) {
    printf("[[starting service]]\n");
    printf("port = %d\n", port);

    if ((status = init(port, backlog)) != STATUS_OK) return status;

    sockaddr_in cli_addr{};
    int addr_len = sizeof(cli_addr);
    int cli_socket;
    vector<thread> threads;

    running = true;
    while (running) {
        cli_socket = accept(socket_fd, (sockaddr *) &cli_addr, (socklen_t *) &addr_len);
        if (cli_socket < 0) {
            fprintf(stderr, "failed to accept new connection");
            continue;
        }
        ConnPtr conn = std::make_shared<Connection>(cli_socket);
        connections.push_back(conn);
        threads.push_back(thread(&VideoService::handleConnection, this, conn));
    }

    return status;
}

void VideoService::handleConnection(ConnPtr conn) {
    char buffer[1024];
    conn->recv(buffer, 1024);
    printf("login = %s\n", buffer);
}

int VideoService::init(int port, int backlog) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "failed to open socket\n");
        return STATUS_ERR_SOCKET;
    }
    if (::bind(socket_fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
        fprintf(stderr, "failed to bind socket\n");
        return STATUS_ERR_SOCKET;
    }
    if (listen(socket_fd, backlog) < 0) {
        fprintf(stderr, "failed to listen on socket\n");
        return STATUS_ERR_SOCKET;
    }
    return STATUS_OK;
}

int VideoService::getStatus() const {
    return status;
}
