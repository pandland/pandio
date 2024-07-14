#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "net.h"
#include "http_request.h"
#include "http_parser.h"
#include "logger.h"
#include "timer.h"

#define DEFAULT_PORT 8000
#define DEFAULT_WORKERS 4
#define BUFFER_SIZE 1024

void request_handler(lxe_connection_t *conn) {
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

    http_request_t *req = http_request_alloc();
    req->connection = conn;

    int status = http_parse(req, buffer);

    if (status != 0) {
      log_err("Parsing failure with status: %d", status);
      lxe_remove_event(&conn->event, conn->fd);
      close(conn->fd);
      http_request_free(req);
      free(conn);
      return;
    }

    int worker_id = *(int*)(conn->listener->data);
    log_info("{ worker: %d, method: %s, path: %s }", worker_id, map_method(req->method), req->path);
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 22\r\nContent-Type: text/html\r\n\r\n<h1>Hello world!</h1>\n";
    send(conn->fd, response, strlen(response), 0);
    //free(response);
    lxe_remove_event(&conn->event, conn->fd);

    close(conn->fd);

    http_request_free(req);
    free(conn);
}

void mytimeout(lxe_timer_t *mytimer) {
    log_info("Timer expired");
    lxe_timer_destroy(mytimer);
}

void acceptor(lxe_connection_t *conn) {
    conn->ondata = request_handler;
    log_info("New connection accepted");
    lxe_timer_t *mytimer = lxe_timer_init(conn->event.ctx);
    lxe_timer_start(mytimer, mytimeout, 3000);
}

void worker(int id, int port) {
    lxe_io_t ctx = lxe_init();
    lxe_listener_t *listener = lxe_listen(&ctx, port, acceptor);

    listener->data = &id;
    lxe_run(&ctx);
}

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
