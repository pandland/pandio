#pragma once
#include <stdint.h>
#include "common.h"
#include "heap.h"

#if defined(__linux__)
typedef int pnd_fd_t;
#endif

struct pnd_io 
{
  pnd_fd_t poll_handle;
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
  int flags;
  pnd_io_t *ctx;
  void (*callback)(struct pnd_event*, int events);
};
