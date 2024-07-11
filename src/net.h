#include "event.h"

typedef struct lxe_listener {
    int fd;
    void *ctx;
    lxe_event_t event
} lxe_listener_t;

typedef struct lxe_connection {
    int fd;
    void (*ondata)(struct lxe_connection *);
    void (*onclose)(struct lxe_connection *);
    void *data;     // pointer to the higher level protocol object
    lxe_listener_t *listener;
    lxe_event_t event;
} lxe_connection_t;

lxe_listener_t lxe_create_listener(int, void*);
void lxe_listen(lxe_listener_t*, void (*)(lxe_connection_t*));
