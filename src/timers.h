#pragma once
#include <limits.h>
#include <stdint.h>
#include "heap.h"
#include "core.h"

struct pd_timer;
typedef void (*pd_timer_callback_t)(struct pd_timer*);

struct pd_timer {
    pd_io_t *ctx;
    uint64_t timeout;
    uint64_t interval;
    bool active;
    void *data;       // custom data for callback
    struct heap_node hnode;
    pd_timer_callback_t on_timeout;
};

typedef struct pd_timer pd_timer_t;

void pd_timer_init(pd_io_t*, pd_timer_t*);

void pd_timer_start(pd_timer_t*, pd_timer_callback_t, uint64_t);

void pnd_timer_repeat(pd_timer_t*, pd_timer_callback_t, uint64_t);

/* safe to call many times */
void pd_timer_stop(pd_timer_t*);

void pd_timers_heap_init(pd_io_t*);

void pd_timers_run(pd_io_t*);

int pd_timers_next(pd_io_t*);
