#pragma once

#include <fcntl.h>
#include <sys/epoll.h>
#include <stdint.h>
#include <stdio.h>

#include "queue.h"
#include "heap.h"

#define MAX_EVENTS 128

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lx_io {
  int epoll_fd;
  struct heap timers;
  uint64_t now;
  uint64_t handles;             // amount of active handles (timers, sockets etc.)
  struct queue pending_closes;
} lx_io_t;

typedef struct lx_event {
  lx_io_t *ctx;
  int flags;
  void (*read)(struct lx_event*);
  void (*write)(struct lx_event*);
} lx_event_t;

/* initializes the event loop */
lx_io_t lx_init();
/* adds event to the epoll */
void lx_add_event(lx_event_t *, int);
void lx_set_read_event(lx_event_t*, int);
void lx_set_write_event(lx_event_t*, int);
void lx_stop_reading(lx_event_t*, int);
void lx_stop_writing(lx_event_t*, int);
/* removes event from the epoll and destroy it */
void lx_remove_event(lx_event_t*, int);
void lx_make_nonblocking(int fd);
void lx_run(lx_io_t*);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
