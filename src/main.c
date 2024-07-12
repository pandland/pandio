#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "net.h"

#define DEFAULT_PORT 8000
#define DEFAULT_WORKERS 4

#define MAX_EVENTS 128
#define BUFFER_SIZE 1024

void echo_handler(lxe_connection_t *conn) {
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(conn->fd, buffer, BUFFER_SIZE, 0);
    recv(conn->fd, buffer, 1024, 0);

    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (bytes_read == 0) {
        close(conn->fd);
        return;
    }

    buffer[bytes_read] = '\0';

    printf("Received: %s", buffer);
    send(conn->fd, buffer, bytes_read, 0);
}

void acceptor(lxe_connection_t *conn) {
    conn->ondata = echo_handler;
}

int main() {
    lxe_io_t ctx = lxe_init();
    lxe_listen(&ctx, DEFAULT_PORT, acceptor);
    lxe_run(&ctx);

    return 0;
}
