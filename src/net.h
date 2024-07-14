#pragma once
#include "event.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

typedef int socket_t;
typedef struct lxe_connection lxe_connection_t;

typedef struct lxe_listener {
    socket_t fd;
    void *data;
    lxe_event_t event;
    void (*onaccept)(struct lxe_connection *);
} lxe_listener_t;

typedef struct lxe_connection {
    socket_t fd;
    void (*ondata)(struct lxe_connection *);
    void (*onclose)(struct lxe_connection *);
    void *data;     // pointer to the higher level protocol object
    lxe_listener_t *listener;
    lxe_event_t event;
} lxe_connection_t;


lxe_listener_t *lxe_listen(lxe_io_t *ctx, int port, void (*)(lxe_connection_t*));
lxe_connection_t *lxe_connection_init(lxe_io_t *ctx, socket_t fd);
lxe_listener_t *lxe_listener_init(lxe_io_t *ctx, socket_t lfd);
void lxe_listener_handler(lxe_event_t *event);
void lxe_connection_handler(lxe_event_t *event);
