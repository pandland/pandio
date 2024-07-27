#pragma once
#include "event.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define LX_NET_BUFFER_SIZE 8192

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
    size_t size;
    char buf[LX_NET_BUFFER_SIZE]; // TODO: define it as flexible array member?
    // output buffers will be replaced with chain of buffers (queue of buffers)
    char *output;
    size_t output_size;
    size_t written;
} lx_connection_t;

#define lx_conn_ctx(conn_ptr) conn_ptr->event.ctx

lx_listener_t *lx_listen(lx_io_t *ctx, int port, void (*)(lx_connection_t*));
lx_connection_t *lx_connection_init(lx_io_t *ctx, socket_t fd);
lx_listener_t *lx_listener_init(lx_io_t *ctx, socket_t lfd);
void lx_listener_handler(lx_event_t *event);
void lx_connection_read(lx_event_t *event);
void lx_connection_write(lx_event_t *event);
void lx_close(lx_connection_t *conn);
int lx_write(lx_connection_t *conn, char *buf, size_t size);
