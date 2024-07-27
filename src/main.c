#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "http/http.h"
#include "logger.h"

#define DEFAULT_PORT 8000
#define DEFAULT_WORKERS 4

void handle_onrequest(http_request_t *req) {}

void worker(int id, int port) {
    lx_io_t ctx = lx_init();
    lx_http_t http = lx_http_init(DEFAULT_PORT, handle_onrequest);
    lx_http_listen(&ctx, &http);
    log_info("Server is listening on port %d", DEFAULT_PORT);
    lx_run(&ctx);
}

void timer_callback(lx_timer_t *timer) {
    printf("Callback\n");
}

void timer_once(lx_timer_t *timer) {
    printf("Should happen only once\n");
}

void timer_test() {
    lx_io_t ctx = lx_init();
    lx_timer_t timer;
    lx_timer_init(&ctx, &timer);
    lx_timer_start(&timer, timer_once, 5 * 1000);

    lx_timer_t timer2;
    lx_timer_init(&ctx, &timer2);
    lx_timer_repeat(&timer2, timer_callback, 3 * 1000);

    lx_run(&ctx);
}

int main() {
    timer_test();
    //worker(1, 8000);
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