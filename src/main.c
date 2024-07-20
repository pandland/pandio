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

void request_handler(lx_connection_t *conn) {
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(conn->fd, buffer, BUFFER_SIZE, 0);
    recv(conn->fd, buffer, 1024, 0);

    if (bytes_read == -1) {
        perror("read");
        lx_close(conn);
        return;
    }

    if (bytes_read == 0) {
        //close(conn->fd);
        lx_close(conn);
        return;
    }

    http_request_t *req = conn->data;
    lx_buf_t buf = { .buf = buffer, .size = bytes_read };
    int status = lx_http_parser_exec(&req->parser, &buf);

    if (status != 0) {
      log_err("Parsing failure: %s", lx_http_map_code(status));
      lx_close(conn);
      return;
    }

    req->path = slice_to_cstr(req->parser.uri);
    //req->method = slice_to_cstr(req->parser.method);
    int worker_id = *(int*)(conn->listener->data);
    //log_info("{ worker: %d, method: %s, path: %s }", worker_id, http_map_method(req->method), req->path);
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 22\r\nContent-Type: text/html\r\n\r\n<h1>Hello world!</h1>\n";
    send(conn->fd, response, strlen(response), 0);

    lx_close(conn);
}

void mytimeout(lx_timer_t *mytimer) {
    log_warn("Connection timed out");
    lx_connection_t *conn = mytimer->data;
    lx_close(conn);
}

void handle_close(lx_connection_t *conn) {
    //log_info("Closing HTTP connection");
    http_request_t *req = conn->data;
    if (!req) {
        return;
    }

    lx_timer_stop(&req->timeout);
    http_request_free(req);
}

void acceptor(lx_connection_t *conn) {
    //log_info("New connection accepted");

    conn->ondata = request_handler;
    conn->onclose = handle_close;

    http_request_t *req = http_request_alloc();
    req->connection = conn;
    lx_timer_init(conn->event.ctx, &req->timeout);

    conn->data = req;
    req->timeout.data = conn;
    lx_timer_start(&req->timeout, mytimeout, 10 * 1000);
}

void worker(int id, int port) {
    lx_io_t ctx = lx_init();
    lx_listener_t *listener = lx_listen(&ctx, port, acceptor);

    listener->data = &id;
    lx_run(&ctx);
}

/*int main() {
    worker(1, 8000);
    return 0;
}*/

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
