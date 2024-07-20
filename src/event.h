#pragma once

#include <fcntl.h>
#include <sys/epoll.h>
#include <stdint.h>

#include "heap.h"

#define MAX_EVENTS 128

typedef struct lx_io {
  int epoll_fd;
  struct heap timers;
  uint64_t now;
} lx_io_t;

typedef struct lx_event {
  lx_io_t *ctx;
  void (*handler)(struct lx_event *); 
  void *data;
} lx_event_t;

/* initializes the event loop */
lx_io_t lx_init();
/* adds event to the epoll */
void lx_add_event(lx_event_t*, int);
/* removes event from the epoll and destroy it */
void lx_remove_event(lx_event_t*, int);
void lx_make_nonblocking(int fd);
void lx_run(lx_io_t*);
