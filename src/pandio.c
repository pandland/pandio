#include "pandio.h"
#include "timers.h"
#include "unix/poll.h"
#include <sys/eventfd.h>
#include "unix/threadpool.h"

void pnd_io_init(pnd_io_t *ctx) 
{
  ctx->poll_handle = 0;
  ctx->handles = 0;
  ctx->now = 0;
  ctx->task_signal = eventfd(0, 0);
  
  pnd_timers_heap_init(ctx);
  queue_init(&ctx->pending_closes);
  queue_init(&ctx->tasks_done);
  pnd_poll_init(ctx);

  pnd_set_nonblocking(ctx->task_signal);

  pnd_event_t *task_event = malloc(sizeof(pnd_event_t));
  pnd_init_event(task_event);
  task_event->ctx = ctx;
  task_event->callback = pnd_work_done_io;
  pnd_add_event_readable(task_event, ctx->task_signal);

  pnd_workers_init();
}


void pnd_tcp_pending_close(pnd_io_t *ctx)
{
  while (!queue_empty(&ctx->pending_closes)) {
    struct queue_node *next = queue_pop(&ctx->pending_closes);
    pnd_tcp_t *stream = container_of(next, pnd_tcp_t, close_qnode);

    if (stream->on_close)
      stream->on_close(stream);
  }
}

void pnd_io_run(pnd_io_t *ctx) 
{
  int timeout = pnd_timers_next(ctx);
  
  while (ctx->handles > 0) {
    pnd_poll_run(ctx, timeout);
    pnd_timers_run(ctx);
    timeout = pnd_timers_next(ctx);
    pnd_tcp_pending_close(ctx);
  }
}
