#include "core.h"
#include "tcp.h"
#include "internal.h"

void pd_tcp_pending_close(pd_io_t *ctx) {
    while (!queue_empty(&ctx->pending_closes)) {
        struct queue_node *next = queue_pop(&ctx->pending_closes);
        pd_tcp_t *stream = container_of(next, pd_tcp_t, close_qnode);

        if (stream->on_close)
            stream->on_close(stream);
    }
}
