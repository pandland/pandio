#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "net.h"
#include "http_request.h"
#include "http_parser.h"
#include "logger.h"

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

    log_info("{ method: %s, path: %s }", map_method(req->method), req->path);
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 22\r\nContent-Type: text/html\r\n\r\n<h1>Hello world!</h1>\n";
    send(conn->fd, response, strlen(response), 0);
    //free(response);
    lxe_remove_event(&conn->event, conn->fd);

    close(conn->fd);

    http_request_free(req);
    free(conn);
}

void acceptor(lxe_connection_t *conn) {
    conn->ondata = echo_handler;
}

int main() {
    int port = DEFAULT_PORT;
    lxe_io_t ctx = lxe_init();
    lxe_listen(&ctx, port, acceptor);
    
    log_info("Server started on port: %d", port);
    lxe_run(&ctx);

    return 0;
}
