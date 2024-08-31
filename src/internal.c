#include "pandio/core.h"
#include "pandio/tcp.h"
#include "internal.h"
#include <fcntl.h>

void pd__tcp_pending_close(pd_io_t *ctx) {
    while (!queue_empty(&ctx->pending_closes)) {
        struct queue_node *next = queue_pop(&ctx->pending_closes);
        pd_tcp_t *stream = container_of(next, pd_tcp_t, close_qnode);

        if (stream->on_close)
            stream->on_close(stream);
    }
}


#ifndef _WIN32

int pd__set_nonblocking(pd_fd_t fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
        return -1;

    return 0;
}

#endif
