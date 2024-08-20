#include <stdio.h>
#include "pandio.h"

void handle_connection(pd_tcp_server_t *server) {
    printf("Received connection\n");
}

void handle_interval(pd_timer_t *timer) {
    printf("Timer callback\n");
}

int main() {
    pd_io_t *ctx = malloc(sizeof(pd_io_t));
    pd_io_init(ctx);

    pd_timer_t timer;
    pd_timer_init(ctx, &timer);
    pd_timer_repeat(&timer, handle_interval, 2000);

    pd_tcp_server_t *server = malloc(sizeof(pd_tcp_server_t));
    pd_tcp_server_init(ctx, server);
    printf("Starting to listen...\n");
    pd_tcp_listen(server, 5000, handle_connection);

    printf("Running event loop\n");
    fflush(stdout);
    pd_io_run(ctx);
}
