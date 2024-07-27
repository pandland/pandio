#include <stdio.h>
#include <limits.h>

#include "common.h"
#include "timer.h"
#include "logger.h"

uint64_t lx_now() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

int timers_comparator(struct heap_node *a, struct heap_node *b) {
  lx_timer_t *child = timer_get(a);
  lx_timer_t *parent = timer_get(b);

  if (child->timeout < parent->timeout) {
    return 1;
  }

  return 0;
}

void timers_init(lx_io_t *ctx) {
  ctx->timers = heap_init(timers_comparator);
}

void lx_timer_init(lx_io_t *ctx, lx_timer_t *timer) {
  timer->ctx = ctx;
  timer->state = TIMER_NONE;
  timer->data = NULL;
  timer->ontimeout = NULL;
  timer->timeout = 0;
  timer->interval = 0;
  heap_init_node(&timer->hnode);
}

lx_timer_t *lx_timer_alloc(lx_io_t *ctx) {
  lx_timer_t *timer = malloc(sizeof(lx_timer_t));
  lx_timer_init(ctx, timer);

  return timer;
}

void lx_timer_start(lx_timer_t *timer, lx_timer_callback_t ontimeout, uint64_t timeout) {
  timer->state = TIMER_ACTIVE;
  timer->timeout = timer->ctx->now + timeout;
  timer->ontimeout = ontimeout;
  timer->ctx->handles++;
  heap_insert(&timer->ctx->timers, &timer->hnode);
}

void lx_timer_repeat(lx_timer_t *timer, lx_timer_callback_t ontimeout, uint64_t interval) {
  timer->interval = interval;
  lx_timer_start(timer, ontimeout, interval);
}

void lx_timer_stop(lx_timer_t *timer) {
  if (timer->state != TIMER_ACTIVE)
    return;

  timer->state = TIMER_STOPPED;
  timer->ctx->handles--;
  heap_remove(&timer->ctx->timers, &timer->hnode);
}

void lx_timer_destroy(lx_timer_t *timer) {
  free(timer);
}

int lx_timers_run(lx_io_t *ctx) {
  lx_timer_t *min;

  while (true) {
    min = timer_get(ctx->timers.root);
    if (min == NULL)
      break;

    if (min->timeout > ctx->now)
      break;
    
    lx_timer_stop(min);
    min->ontimeout(min);

    if (min->interval != 0)
      lx_timer_start(min, min->ontimeout, min->interval);
  }

  // return next timeout for event selector (like epoll):
  min = timer_get(ctx->timers.root);
  if (min == NULL)
    return -1;

  uint64_t next_timeout = min->timeout - ctx->now;

  return (next_timeout >= INT_MAX) ? INT_MAX : (int)(next_timeout);
}
