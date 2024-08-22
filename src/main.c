#include <stdio.h>
#include "pandio.h"
#include <string.h>

int counter = 1;

void handle_write(pd_write_t *write_op, int status) {
    printf("Written successfully with status: %d\n", status);
    fflush(stdout);
    free(write_op->data.buf);
    free(write_op);
}


void handle_connection(pd_tcp_server_t *server, pd_socket_t socket, int status) {
    pd_tcp_t *client = malloc(sizeof(pd_tcp_t));
    pd_tcp_init(server->ctx, client);
    pd_tcp_accept(client, socket);
    printf("Received connection #%d\n", counter++);

    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
    pd_write_t *write_op = malloc(sizeof(pd_write_t));
    pd_write_init(write_op, strdup(response), strlen(response), handle_write);

    pd_tcp_write(client, write_op);
    // TODO: close connection
}

void handle_interval(pd_timer_t *timer) {
    printf("Timer callback\n");
}

int main() {
    pd_io_t *ctx = malloc(sizeof(pd_io_t));
    pd_io_init(ctx);

    pd_timer_t timer;
    pd_timer_init(ctx, &timer);
    pd_timer_repeat(&timer, handle_interval, 50000);

    pd_tcp_server_t *server = malloc(sizeof(pd_tcp_server_t));
    pd_tcp_server_init(ctx, server);
    printf("Starting to listen...\n");
    pd_tcp_listen(server, 5000, handle_connection);
    pd_io_run(ctx);
}
