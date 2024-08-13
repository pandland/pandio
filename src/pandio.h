#pragma once
#include <stdint.h>
#include "common.h"
#include "heap.h"
#include "queue.h"

#if defined(__linux__)
typedef int pnd_fd_t;
#endif

struct pnd_io {
  pnd_fd_t poll_handle;
  size_t handles;
  uint64_t now;
  struct heap timers;
};

typedef struct pnd_io pnd_io_t;

enum {
  PND_READABLE = 1,
  PND_WRITABLE = 2,
  PND_CLOSE = 4
};

struct pnd_event {
  unsigned flags;
  pnd_io_t *ctx;
  void (*callback)(struct pnd_event*, unsigned events);
};

typedef struct pnd_event pnd_event_t;

enum {
  PND_TCP_NONE = 0,  // initial state, when socket is not initialized yet but zeroed struct exists.
  PND_TCP_ACTIVE,
  PND_TCP_CLOSING,
  PND_TCP_CLOSED
};

struct pnd_tcp {
  pnd_fd_t fd;
  pnd_event_t ev;
  void (*io_handler)(struct pnd_event*, int events);
  int state;
  struct queue writes;
  size_t writes_size;
  // public fields for user to use:
  void *data;
  void (*on_data)(struct pnd_tcp *);
  void (*on_close)(struct pnd_tcp *);
  void (*on_connect)(pnd_fd_t);
};

typedef struct pnd_tcp pnd_tcp_t;
