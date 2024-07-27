#pragma once

#include <time.h>
#include <stdint.h>

#include "heap.h"
#include "queue.h"
#include "event.h"

typedef enum lx_timer_state {
  TIMER_NONE,
  TIMER_ACTIVE,
  TIMER_STOPPED
} lx_timer_state_t;

struct lx_timer;

typedef void (*lx_timer_callback_t)(struct lx_timer*);

typedef struct lx_timer {
  lx_io_t *ctx;
  uint64_t timeout;
  uint64_t interval;
  struct heap_node hnode;
  void *data; // custom data for callback
  lx_timer_callback_t ontimeout;
  lx_timer_state_t state;
} lx_timer_t;

#define timer_get(ptr) \
  (ptr == NULL) ? NULL : container_of(ptr, lx_timer_t, hnode)

uint64_t lx_now();
void timers_init(lx_io_t*);
int timers_comparator(struct heap_node*, struct heap_node*);
void lx_timer_init(lx_io_t*, lx_timer_t*);
lx_timer_t *lx_timer_alloc(lx_io_t*);
void lx_timer_start(lx_timer_t*, lx_timer_callback_t, uint64_t);
void lx_timer_repeat(lx_timer_t*, lx_timer_callback_t, uint64_t);
void lx_timer_stop(lx_timer_t*);
void lx_timer_destroy(lx_timer_t*);
int lx_timers_run(lx_io_t*);
