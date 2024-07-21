#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "http.h"
#include "logger.h"

#define DEFAULT_PORT 8000
#define DEFAULT_WORKERS 4

void handle_onrequest() {}

void worker(int id, int port) {
    lx_io_t ctx = lx_init();
    lx_http_t http = lx_http_init(DEFAULT_PORT, handle_onrequest);
    lx_http_listen(&ctx, &http);
    log_info("Server is listening on port %d", DEFAULT_PORT);
    lx_run(&ctx);
}

int main() {
    worker(1, 8000);
    return 0;
}

/*
int main() {
    int workers = DEFAULT_WORKERS;
    int port = DEFAULT_PORT;

    for (int i = 0; i < workers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            int id = i + 1;
            log_info("Worker #%d with pid %d started", id, getpid());
            worker(id, port);
            exit(EXIT_SUCCESS);
        }
    }
    
    log_info("Server listen on port: %d", port);

    for (int i = 0; i < workers; i++) {
        wait(NULL);
    }

    return 0;
}
*/