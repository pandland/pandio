#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include "ev.h"

typedef int socket_t;

struct tcp_connection_s {
    socket_t fd;
    socket_t lfd; // listener socket fd
    //struct sockaddr_in addr;
    void *data; // pointer to the struct representing higher level protocol like http_request_t
    int (*recv)(struct tcp_connection_s *, char *);
    void (*send)(struct tcp_connection_s *, char *);
    event_t ev;
};

struct tcp_listener_s {
    socket_t fd;
    event_t ev;
};

typedef struct tcp_connection_s tcp_connection_t;
typedef struct tcp_listener_s tcp_listener_t;

socket_t tcp_listen(int port);

tcp_connection_t *tcp_init_conn(ev_loop_t *loop, socket_t fd);

tcp_listener_t *tcp_init_listener(ev_loop_t *loop, socket_t fd);

int tcp_recv(tcp_connection_t *conn, char *bytes);

void tcp_send(tcp_connection_t *conn, char *bytes);
