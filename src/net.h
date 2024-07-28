#pragma once
#include "event.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include "queue.h"

#define LX_NET_BUFFER_SIZE 8192

typedef int socket_t;
typedef struct lx_connection lx_connection_t;

typedef struct lx_listener {
    socket_t fd;
    void *data;
    lx_event_t event;
    void (*onaccept)(struct lx_connection *);
} lx_listener_t;

struct lx_write;

typedef void (*write_cb_t)(struct lx_write*);

typedef struct lx_write {
    const char *buf;
    size_t size;
    size_t written;
    write_cb_t cb;
    void *data;
    struct queue_node qnode;
} lx_write_t;

typedef struct lx_connection {
    socket_t fd;
    void (*ondata)(struct lx_connection *);
    void (*onclose)(struct lx_connection *);
    void *data;     // pointer to the higher level protocol object
    lx_listener_t *listener;
    lx_event_t event;
    size_t size;
    char buf[LX_NET_BUFFER_SIZE]; // TODO: define it as flexible array member?
    struct queue output;
} lx_connection_t;

#define lx_conn_ctx(conn_ptr) conn_ptr->event.ctx

lx_listener_t *lx_listen(lx_io_t *ctx, int port, void (*)(lx_connection_t*));
lx_connection_t *lx_connection_init(lx_io_t *ctx, socket_t fd);
lx_listener_t *lx_listener_init(lx_io_t *ctx, socket_t lfd);
void lx_listener_handler(lx_event_t *event);
void lx_connection_read(lx_event_t *event);
void lx_connection_write(lx_event_t *event);
void lx_close(lx_connection_t *conn);

lx_write_t *lx_write_alloc(const char *buf, size_t size);
int lx_write(lx_write_t*, lx_connection_t *conn, write_cb_t cb);
