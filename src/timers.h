#pragma once
#include <stdint.h>
#include "heap.h"
#include "pandio.h"

struct pnd_timer;
typedef void (*pnd_timer_callback_t)(struct pnd_timer*);

struct pnd_timer {
  pnd_io_t *ctx;
  uint64_t timeout;
  uint64_t interval;
  bool active;
  void *data;       // custom data for callback
  struct heap_node hnode;
  pnd_timer_callback_t on_timeout;
};

typedef struct pnd_timer pnd_timer_t;

void pnd_timer_init(pnd_io_t*, pnd_timer_t*);

void pnd_timer_start(pnd_timer_t*, pnd_timer_callback_t, uint64_t);

void pnd_timer_repeat(pnd_timer_t*, pnd_timer_callback_t, uint64_t);

/* safe to call many times */
void pnd_timer_stop(pnd_timer_t*);

void pnd_timers_heap_init(pnd_io_t*);

void pnd_timers_run(pnd_io_t*);

int pnd_timers_next_timeout(pnd_io_t*);
