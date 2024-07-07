#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "logger.h"
#include "common.h"
#include "tcp.h"
#include "http_parser.h"
#include "http_request.h"

void tcp_accept(event_t *event) {
  tcp_listener_t *listener = container_of(event, tcp_listener_t, ev);

  struct sockaddr_in address;
  int addrlen = sizeof(address);
  socket_t client_fd = accept(listener->fd, (struct sockaddr *)&address, &addrlen);
  if (client_fd < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      return;
    }
    perror("accept");
    exit(EXIT_FAILURE);
  }

  ev_make_nonblocking(client_fd);
  tcp_connection_t *conn = tcp_init_conn(event->loop, client_fd);;
  ev_register(event->loop, client_fd, &conn->ev);
}

tcp_listener_t *tcp_init_listener(ev_loop_t *loop, socket_t fd) {
  tcp_listener_t *listener = malloc(sizeof(tcp_listener_t));
  listener->fd = fd;
  listener->ev = ev_new_event(loop, fd);
  listener->ev.type = EV_LISTENER;
  listener->ev.handler = tcp_accept;
}

socket_t tcp_listen(int port) {
  socket_t listener_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listener_fd <= 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  int opt = 1;
  if (setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

  if (bind(listener_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  ev_make_nonblocking(listener_fd);

  if (listen(listener_fd, SOMAXCONN) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  return listener_fd;
}


#define BUFFER_SIZE 1024

void tcp_handler(event_t *event) {
  tcp_connection_t *conn = container_of(event, tcp_connection_t, ev);

    char buffer[BUFFER_SIZE];
    int bytes_read = recv(conn->fd, buffer, BUFFER_SIZE, 0);
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
    int status = http_parse(req, buffer);

    if (status != 0) {
      log_err("Parsing failure with status: %d", status);
      ev_remove(conn->ev.loop, conn->fd);
      close(conn->fd);
      free(conn);
      return;
    }

    log_info("{ method: %s, path: %s }", map_method(req->method), req->path);
    //printf("Received: %s\n", buffer);

    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 22\r\nContent-Type: text/html\r\n\r\n<h1>Hello world!</h1>\n";
    send(conn->fd, response, strlen(response), 0);
    ev_remove(conn->ev.loop, conn->fd);

    close(conn->fd);

    free(conn);
}

tcp_connection_t *tcp_init_conn(ev_loop_t *loop, socket_t fd) {
  tcp_connection_t *conn = malloc(sizeof(tcp_connection_t));
  conn->fd = fd;
  conn->ev = ev_new_event(loop, fd);
  conn->ev.type = EV_TCP;
  conn->ev.handler = tcp_handler;

  return conn;
}

int tcp_recv(tcp_connection_t *conn, char *bytes);

void tcp_send(tcp_connection_t *conn, char *bytes);
