#include "net.h"
#include "common.h"

lxe_connection_t *lxe_connection_init(lxe_io_t *ctx, socket_t fd) {
  lxe_connection_t *conn = malloc(sizeof(lxe_connection_t));
  conn->fd = fd;
  conn->ondata = NULL;
  conn->onclose = NULL;
  conn->data = NULL;

  conn->event.ctx = ctx;
  conn->event.data = NULL;
  conn->event.handler = lxe_connection_handler;

  return conn;
}

lxe_listener_t *lxe_listener_init(lxe_io_t *ctx, socket_t lfd) {
  lxe_listener_t *listener = malloc(sizeof(lxe_listener_t));
  listener->fd = lfd;
  listener->data = NULL;
  listener->onaccept = NULL;

  listener->event.ctx = ctx;
  listener->event.handler = lxe_listener_handler;
  listener->event.data = NULL;

  return listener;
}

/* epoll event handler for listener */
void lxe_listener_handler(lxe_event_t *event) {
  lxe_listener_t *listener = container_of(event, lxe_listener_t, event);

  struct sockaddr_in address;
  int addrlen = sizeof(address);
  socket_t client_fd = accept(listener->fd, (struct sockaddr *)&address, &addrlen);
  if (client_fd < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      return;
    }
    perror("accept");
    exit(EXIT_FAILURE);
  }

  lxe_make_nonblocking(client_fd);

  lxe_connection_t *conn = lxe_connection_init(event->ctx, client_fd);
  conn->listener = listener;

  lxe_add_event(&conn->event, client_fd);
  if (listener->onaccept != NULL)
    listener->onaccept(conn);
}

/* epoll event handler for connection (EPOLLIN) */
void lxe_connection_handler(lxe_event_t *event) {
    lxe_connection_t *conn = container_of(event, lxe_connection_t, event);
    if (conn->ondata != NULL)
      conn->ondata(conn);
}

lxe_listener_t *lxe_listen(lxe_io_t *ctx, int port, void (*onaccept)(struct lxe_connection *)) {
  socket_t lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lfd <= 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  int opt = 1;
  if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

  if (bind(lfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  lxe_make_nonblocking(lfd);

  if (listen(lfd, SOMAXCONN) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  
  lxe_listener_t *listener = lxe_listener_init(ctx, lfd);
  listener->onaccept = onaccept;

  lxe_add_event(&listener->event, lfd);
  return listener;
}
