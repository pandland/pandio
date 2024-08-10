#include "pandio.h"
#include "poll.h"
#include "common.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#define MAX_EVENTS 1024

uint64_t pnd_now() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

typedef struct pnd_event pnd_event_t;

void pnd_poll_init(pnd_io_t *ctx) {
  ctx->poll_handle = epoll_create1(0);

  if (ctx->poll_handle == -1) {
    perror("epoll");
    exit(EXIT_FAILURE);
  }
}

void pnd_poll_run(pnd_io_t *ctx, int timeout) {
  struct epoll_event events[MAX_EVENTS];
  struct epoll_event ev;
  pnd_event_t *pev;
  int pevents;

  int events_count = epoll_wait(ctx->poll_handle, events, MAX_EVENTS, timeout);
  if (events_count == -1) {
    perror("epoll_wait");
    exit(EXIT_FAILURE);
  }

  ctx->now = pnd_now();
  
  for (int i = 0; i < events_count; ++i) {
    pevents = 0;
    ev = events[i];
    pev = ev.data.ptr;

    if (ev.events & EPOLLIN) {
      pevents |= PND_READABLE;
    }

    if (ev.events & EPOLLOUT)
      pevents |= PND_WRITABLE;

    if (ev.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
      pevents |= PND_CLOSE;
    }

    pev->callback(pev, pevents);
  }
}

void pnd_modify_event(pnd_event_t *event, int fd, uint32_t operation, uint32_t flags) {
  struct epoll_event ev;
  event->flags = flags;

  ev.events = event->flags;
  ev.data.ptr = event;
  if (epoll_ctl(event->ctx->poll_handle, operation, fd, &ev) == -1) {
    perror("pnd_modify_event");
  }
}

void pnd_add_event(pnd_event_t *event, pnd_fd_t fd) {
  event->flags |= EPOLLIN;
  //event->ctx->handles++;
  pnd_modify_event(event, fd, EPOLL_CTL_ADD, event->flags);
}

void pnd_start_reading(pnd_event_t *event, pnd_fd_t fd) {
  event->flags |= EPOLLIN;
  pnd_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void pnd_stop_reading(pnd_event_t *event, pnd_fd_t fd) {
  event->flags &= ~EPOLLIN;
  pnd_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void pnd_start_writing(pnd_event_t *event, pnd_fd_t fd) {
  event->flags |= EPOLLOUT;
  pnd_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void pnd_stop_writing(pnd_event_t *event, pnd_fd_t fd) {
  event->flags &= ~EPOLLOUT;
  pnd_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void pnd_remove_event(pnd_event_t *event, pnd_fd_t fd) {
  //event->ctx->handles--;
  if (epoll_ctl(event->ctx->poll_handle, EPOLL_CTL_DEL, fd, NULL) == -1) {
    perror("pnd_remove_event");
  }
}
void pnd_set_nonblocking(pnd_fd_t fd) {
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
