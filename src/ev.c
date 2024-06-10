#include "ev.h"
#include "common.h"

#include "logger.h"

ev_loop_t ev_loop_init() {
  ev_loop_t loop;
  loop.epoll_fd = epoll_create1(0);
  if (loop.epoll_fd == -1) {
    perror("epoll");
    exit(EXIT_FAILURE);
  }

  return loop;
}

void ev_make_nonblocking(int fd) {
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

void ev_register(ev_loop_t *loop, int fd, event_t *event) {
  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.ptr = event;
  if (epoll_ctl(loop->epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    perror("ev_register");
    exit(EXIT_FAILURE);
  }
}

void ev_remove(ev_loop_t *loop, int fd) {
  if (epoll_ctl(loop->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
    perror("ev_remove");
    exit(EXIT_FAILURE);
  }
}

event_t ev_new_event(ev_loop_t *loop, int fd) {
  event_t ev;
  ev.loop = loop;
  ev.fd = fd;
  return ev;
}

void ev_loop_run(ev_loop_t *loop) {
    struct epoll_event events[MAX_EVENTS];

    while(true) {
        int events_count = epoll_wait(loop->epoll_fd, events, MAX_EVENTS, -1);
        if (events_count == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < events_count; i++) {
            event_t *ev = events[i].data.ptr;
            ev->handler(ev);
        }
    }
}
