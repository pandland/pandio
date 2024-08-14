#include "pandio.h"
#include "timers.h"
#include "unix/poll.h"

void pnd_io_init(pnd_io_t *ctx) 
{
  ctx->poll_handle = 0;
  ctx->handles = 0;
  ctx->now = 0;
  
  pnd_timers_heap_init(ctx);
  queue_init(&ctx->pending_closes);
  pnd_poll_init(ctx);
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
