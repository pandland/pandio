#include "common.h"
#include "event.h"
#include "timer.h"

lx_io_t lx_init() {
  lx_io_t ctx;
  ctx.epoll_fd = epoll_create1(0);
  ctx.now = lx_now(); // it will be updated with every cycle anyway
  ctx.handles = 0;

  if (ctx.epoll_fd == -1) {
    perror("epoll");
    exit(EXIT_FAILURE);
  }

  timers_init(&ctx);

  return ctx;
}

void lx_make_nonblocking(int fd) {
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

void lx_modify_event(lx_event_t *event, int fd, uint32_t operation, uint32_t flags) {
  struct epoll_event ev;
  event->flags = flags;

  ev.events = event->flags;
  ev.data.ptr = event;
  if (epoll_ctl(event->ctx->epoll_fd, operation, fd, &ev) == -1) {
    perror("lx_modify_event");
  }
}

void lx_add_event(lx_event_t *event, int fd) {
  event->flags |= EPOLLIN;
  event->ctx->handles++;
  lx_modify_event(event, fd, EPOLL_CTL_ADD, event->flags);
}

void lx_set_read_event(lx_event_t *event, int fd) {
  event->flags |= EPOLLIN;
  lx_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void lx_set_write_event(lx_event_t *event, int fd) {
  event->flags |= EPOLLOUT;
  lx_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void lx_stop_reading(lx_event_t *event, int fd) {
  event->flags &= ~EPOLLIN;
  lx_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void lx_stop_writing(lx_event_t *event, int fd) {
  event->flags &= ~EPOLLOUT;
  lx_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void lx_remove_event(lx_event_t *event, int fd) {
  event->ctx->handles--;
  if (epoll_ctl(event->ctx->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
    perror("lx_remove_event");
  }
}

void lx_run(lx_io_t *ctx) {
    struct epoll_event events[MAX_EVENTS];
    int epoll_timeout = lx_timers_run(ctx);

    while(ctx->handles > 0) {
        int events_count = epoll_wait(ctx->epoll_fd, events, MAX_EVENTS, epoll_timeout);
        if (events_count == -1) {
          perror("epoll_wait");
          exit(EXIT_FAILURE);
        }

        ctx->now = lx_now();

        for (int i = 0; i < events_count; i++) {
          lx_event_t *event = events[i].data.ptr;
          if (events[i].events & EPOLLIN) {
            if (event->read) {
              event->read(event);
            }
          }

          if (events[i].events & EPOLLOUT) {
            if (event->write) {
              event->write(event);
            }
          }
        }

        epoll_timeout = lx_timers_run(ctx);
    }
}
