#include <time.h>
#include <stdint.h>

#include "heap.h"
#include "queue.h"
#include "event.h"


typedef enum lxe_timer_state {
  TIMER_NONE,
  TIMER_ACTIVE,
  TIMER_STOPPED
} lxe_timer_state_t;

struct lxe_timer;

typedef void (*lxe_timer_callback_t)(struct lxe_timer*);

typedef struct lxe_timer {
  lxe_io_t *ctx;
  uint64_t timeout;
  struct heap_node hnode;
  void *data; // custom data for callback
  lxe_timer_callback_t ontimeout;
  lxe_timer_state_t state;
} lxe_timer_t;

#define timer_get(ptr) \
  (ptr == NULL) ? NULL : container_of(ptr, lxe_timer_t, hnode)

uint64_t lxe_now();
void timers_init(lxe_io_t*);
int timers_comparator(struct heap_node*, struct heap_node*);
lxe_timer_t *lxe_timer_init(lxe_io_t*);
void lxe_timer_start(lxe_timer_t*, lxe_timer_callback_t, uint64_t);
void lxe_timer_stop(lxe_timer_t*);
void lxe_timer_destroy(lxe_timer_t*);
int lxe_timers_run(lxe_io_t*);
