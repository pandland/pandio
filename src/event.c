#include "common.h"
#include "event.h"
#include "timer.h"

lxe_io_t lxe_init() {
  lxe_io_t ctx;
  ctx.epoll_fd = epoll_create1(0);
  ctx.now = lxe_now(); // it will be updated with every cycle anyway
  if (ctx.epoll_fd == -1) {
    perror("epoll");
    exit(EXIT_FAILURE);
  }

  timers_init(&ctx);

  return ctx;
}

void lxe_make_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

void lxe_add_event(lxe_event_t *event, int fd) {
  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.ptr = event;
  if (epoll_ctl(event->ctx->epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    perror("lxe_add_event");
    exit(EXIT_FAILURE);
  }
}

void lxe_remove_event(lxe_event_t *event, int fd) {
  if (epoll_ctl(event->ctx->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
    perror("lxe_remove_event");
    exit(EXIT_FAILURE);
  }
}

void lxe_run(lxe_io_t *ctx) {
    struct epoll_event events[MAX_EVENTS];
    int epoll_timeout = -1;

    while(true) {
        int events_count = epoll_wait(ctx->epoll_fd, events, MAX_EVENTS, epoll_timeout);
        if (events_count == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        ctx->now = lxe_now();

        for (int i = 0; i < events_count; i++) {
            lxe_event_t *event = events[i].data.ptr;
            event->handler(event);
        }

        epoll_timeout = lxe_timers_run(ctx);
    }
}
