#pragma once

#include <fcntl.h>
#include <sys/epoll.h>
#include "event.h"

#define MAX_EVENTS 128

typedef struct lxe_io {
  int epoll_fd; 
} lxe_io_t;

typedef struct lxe_event {
  lxe_io_t *ctx;
  void (*handler)(struct lxe_event *); 
  void *data;
} lxe_event_t;

/* initializes the event loop */
lxe_io_t lxe_init();
/* adds event to the epoll */
void lxe_add_event(lxe_event_t*, int);
/* removes event from the epoll and destroy it */
void lxe_remove_event(lxe_event_t*, int);
void lxe_make_nonblocking(int fd);
void lxe_run(lxe_io_t*);
