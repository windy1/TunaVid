#include <iostream>
#include "Server/VideoService.h"
#include "Client/VideoClient.h"

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "--server") == 0) {
        return VideoService().start(8000);
    } else if (argc == 4 && strcmp(argv[1], "--client") == 0) {
        return VideoClient().start(argc, argv);
    } else {
        fprintf(stderr, "unknown option: %s\n", argv[1]);
        return 2;
    }
}
