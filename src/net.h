#pragma once
#include "event.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

typedef int socket_t;
typedef struct lx_connection lx_connection_t;

typedef struct lx_listener {
    socket_t fd;
    void *data;
    lx_event_t event;
    void (*onaccept)(struct lx_connection *);
} lx_listener_t;

typedef struct lx_connection {
    socket_t fd;
    void (*ondata)(struct lx_connection *);
    void (*onclose)(struct lx_connection *);
    void *data;     // pointer to the higher level protocol object
    lx_listener_t *listener;
    lx_event_t event;
    /* buffer is stored in TCP connection object, 
     * because I want to support multiple protocols (http and ws), each with own struct
     * so I can reuse same buffer to save memory usage
     */
    size_t size;
    char buf[8192]; // TODO: define it as flexible array member?
} lx_connection_t;

lx_listener_t *lx_listen(lx_io_t *ctx, int port, void (*)(lx_connection_t*));
lx_connection_t *lx_connection_init(lx_io_t *ctx, socket_t fd);
lx_listener_t *lx_listener_init(lx_io_t *ctx, socket_t lfd);
void lx_listener_handler(lx_event_t *event);
void lx_connection_handler(lx_event_t *event);
void lx_close(lx_connection_t *conn);
