/* Copyright (c) Michał Dziuba
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "core.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include "timers.h"
#include <stdio.h>
#include "internal.h"
#include "threadpool.h"


uint64_t pd_now() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}


void pd_io_init(pd_io_t *ctx) {
    ctx->poll_fd = epoll_create1(0);

    if (ctx->poll_fd == -1) {
        abort();
    }

    ctx->now = pd_now();
    pd_timers_heap_init(ctx);

    ctx->task_signal = malloc(sizeof(pd_notifier_t));
    pd_notifier_init(ctx, ctx->task_signal);
    ctx->task_signal->handler = pd__task_done;
    ctx->task_signaled = false;

    queue_init(&ctx->finished_tasks);
}


void pd_event_init(pd_event_t *event) {
    event->handler = NULL;
    event->flags = 0;
    event->data = NULL;
}


void pd_event_modify(pd_io_t *ctx, pd_event_t *event, int fd, int operation, unsigned flags) {
    struct epoll_event ev;
    event->flags = flags;

    ev.events = event->flags;
    ev.data.ptr = event;
    if (epoll_ctl(ctx->poll_fd, operation, fd, &ev) == -1) {
        perror("pnd_modify_event");
    }
}


void pd_event_add_readable(pd_io_t *ctx, pd_event_t *event, pd_fd_t fd) {
    event->flags |= EPOLLIN;
    //event->ctx->handles++;
    pd_event_modify(ctx, event, fd, EPOLL_CTL_ADD, event->flags);
}


void pd_event_add_writable(pd_io_t *ctx, pd_event_t *event, pd_fd_t fd) {
    event->flags |= EPOLLOUT;
    //event->ctx->handles++;
    pd_event_modify(ctx, event, fd, EPOLL_CTL_ADD, event->flags);
}


void pd_event_del(pd_io_t *ctx, pd_event_t *event, pd_fd_t fd) {
    //ctx->handles--;
    if (epoll_ctl(ctx->poll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        perror("pnd_remove_event");
    }
}


void pd_event_read_start(pd_io_t *ctx, pd_event_t *event, pd_fd_t fd) {
    event->flags |= EPOLLIN;
    pd_event_modify(ctx, event, fd, EPOLL_CTL_MOD, event->flags);
}


void pd_event_read_stop(pd_io_t *ctx, pd_event_t *event, pd_fd_t fd) {
    event->flags &= ~EPOLLIN;
    pd_event_modify(ctx, event, fd, EPOLL_CTL_MOD, event->flags);
}


/* remove WRITE interest and set only READ interest via single system call.
 * equal to calling: pd_event_write_stop and pd_event_read_start
 */
void pd_event_read_only(pd_io_t *ctx, pd_event_t *event, pd_fd_t fd) {
    event->flags |= EPOLLIN;
    event->flags &= ~EPOLLOUT;
    pd_event_modify(ctx, event, fd, EPOLL_CTL_MOD, event->flags);
}


void pd_event_write_start(pd_io_t *ctx, pd_event_t *event, pd_fd_t fd) {
    event->flags |= EPOLLOUT;
    pd_event_modify(ctx, event, fd, EPOLL_CTL_MOD, event->flags);
}


void pd_event_write_stop(pd_io_t *ctx, pd_event_t *event, pd_fd_t fd) {
    event->flags &= ~EPOLLOUT;
    pd_event_modify(ctx, event, fd, EPOLL_CTL_MOD, event->flags);
}


int pd_set_nonblocking(pd_fd_t fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl");
        return -1;
    }

    return 0;
}

#define MAX_EVENTS 1024

void pd_io_run(pd_io_t *ctx) {
    struct epoll_event events[MAX_EVENTS];
    struct epoll_event ev;
    int timeout = pd_timers_next(ctx);
    pd_event_t *pev;

    // TODO: break loop if there is no active handles
    while (true) {
        int events_count =
                epoll_wait(ctx->poll_fd, events, MAX_EVENTS, timeout);

        if (events_count == -1 && errno == EINTR) {
            // interrupted by signal
            continue;
        }

        if (events_count == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        ctx->now = pd_now();

        for (int i = 0; i < events_count; ++i) {
            ev = events[i];
            unsigned pevents = 0;
            pev = ev.data.ptr;

            if (ev.events & EPOLLIN)
                pevents |= PD_POLLIN;

            if (ev.events & EPOLLOUT)
                pevents |= PD_POLLOUT;

            if (ev.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                // usually we have to close handle in this scenario, so let's simplify detection
                pevents |= PD_CLOSE;

                if (ev.events & EPOLLERR)
                    pevents |= PD_POLLERR;

                if (ev.events & EPOLLHUP)
                    pevents |= PD_POLLHUP;

                if (ev.events & EPOLLRDHUP)
                    pevents |= PD_POLLRDHUP;

            }

            pev->handler(pev, pevents);
        }

        pd_timers_run(ctx);
        timeout = pd_timers_next(ctx);
        pd_tcp_pending_close(ctx);
    }
}
