#pragma once

#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_EVENTS 128

struct ev_loop_s {
  int epoll_fd;
};
typedef struct ev_loop_s ev_loop_t;

enum {
  EV_TCP = 0,
  EV_LISTENER
} typedef EV_TYPE;

struct event_s {
  int fd;
  EV_TYPE type;
  ev_loop_t *loop;
  void (*handler)(struct event_s *);
};
typedef struct event_s event_t;

ev_loop_t ev_loop_init();

void ev_register(ev_loop_t *loop, int fd, event_t *event);

void ev_make_nonblocking(int fd);

void ev_remove(ev_loop_t *loop, int fd);

void ev_loop_run(ev_loop_t *loop);

event_t ev_new_event(ev_loop_t *loop, int fd);
