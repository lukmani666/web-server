#include "server.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : 8080;

    start_server(port);

    return 0;
}