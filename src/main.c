#include <stdbool.h>
#include <stdlib.h>

#include "http_request.h"
#include "logger.h"
#include "ev.h"
#include "tcp.h"

#define DEFAULT_PORT 8080
#define DEFAULT_WORKERS 4

#define MAX_EVENTS 128
#define BUFFER_SIZE 1024

void worker(socket_t lfd) {
    ev_loop_t loop = ev_loop_init();
    tcp_listener_t *listener = tcp_init_listener(&loop, lfd);
    ev_register(&loop, lfd, &listener->ev);
    ev_loop_run(&loop);
}


int getenv_int(const char *name, int default_value) {
    char *value = getenv(name);
    if (value == NULL) {
        return default_value;
    }
    return atoi(value);
}

int main() {
    int port = getenv_int("PORT", DEFAULT_PORT);
    int workers = getenv_int("WORKERS", DEFAULT_WORKERS);

    socket_t lfd = tcp_listen(port);

    for (int i = 0; i < workers; i++) {
        int pid = fork();
        if (pid == 0) {
            log_info("Worker #%d with pid %d started", i + 1, getpid());
            worker(lfd);
            exit(EXIT_SUCCESS);
        }
    }

    log_info("Listening on port %d", port);

    while(true) {
        // waiting for signals etc...
    }
}
