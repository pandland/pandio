#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>

int init_epoll() {
  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("epoll");
    exit(EXIT_FAILURE);
  }

  return epoll_fd;
}

void add_to_epoll(int epoll_fd, int fd) {
  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    perror("epoll");
    exit(EXIT_FAILURE);
  }
}

void remove_from_epoll(int epoll_fd, int fd) {
  if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
    perror("epoll");
    exit(EXIT_FAILURE);
  }
}
