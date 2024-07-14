#include <stdio.h>
#include <limits.h>

#include "common.h"
#include "timer.h"

uint64_t lxe_now() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

int timers_comparator(struct heap_node *a, struct heap_node *b) {
  lxe_timer_t *child = timer_get(a);
  lxe_timer_t *parent = timer_get(b);
  //printf("%d vs %d\n", parent->timeout, child->timeout);
  if (child->timeout < parent->timeout) {
    return 1;
  }

  return 0;
}

void timers_init(lxe_io_t *ctx) {
  ctx->timers = heap_init(timers_comparator);
}

lxe_timer_t *lxe_timer_init(lxe_io_t *ctx) {
  lxe_timer_t *timer = malloc(sizeof(lxe_timer_t));

  timer->ctx = ctx;
  timer->state = TIMER_NONE;
  timer->data = NULL;;
  timer->ontimeout = NULL;
  timer->timeout = 0;
  heap_init_node(&timer->hnode);

  return timer;
}

void lxe_timer_start(lxe_timer_t *timer, lxe_timer_callback_t ontimeout, uint64_t timeout) {
  timer->state = TIMER_ACTIVE;
  timer->timeout = timer->ctx->now + timeout;
  timer->ontimeout = ontimeout;
  heap_insert(&timer->ctx->timers, &timer->hnode);
}

void lxe_timer_stop(lxe_timer_t *timer) {
  if (timer->state != TIMER_ACTIVE)
    return;

  timer->state = TIMER_STOPPED;
  heap_remove(&timer->ctx->timers, &timer->hnode);
}

void lxe_timer_destroy(lxe_timer_t *timer) {
  free(timer);
}

int lxe_timers_run(lxe_io_t *ctx) {
  lxe_timer_t *min;

  while (true) {
    min = timer_get(ctx->timers.root);
    if (min == NULL)
      break;

    if (min->timeout > ctx->now)
      break;
    
    lxe_timer_stop(min);
    min->ontimeout(min);
  }

  // return next timeout for event selector (like epoll):
  min = timer_get(ctx->timers.root);
  if (min == NULL)
    return -1;

  uint64_t next_timeout = min->timeout - ctx->now;

  return (next_timeout >= INT_MAX) ? INT_MAX : (int)(next_timeout);
}
