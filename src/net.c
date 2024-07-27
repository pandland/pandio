#include "net.h"
#include "common.h"

lx_connection_t *lx_connection_init(lx_io_t *ctx, socket_t fd) {
  lx_connection_t *conn = malloc(sizeof(lx_connection_t));
  conn->fd = fd;
  conn->ondata = NULL;
  conn->onclose = NULL;
  conn->data = NULL;
  conn->size = 0;

  conn->output = NULL;
  conn->written = 0;
  conn->output_size = 0;

  conn->event.ctx = ctx;
  conn->event.flags = 0;
  conn->event.read = lx_connection_read;
  conn->event.write = lx_connection_write;

  return conn;
}

lx_listener_t *lx_listener_init(lx_io_t *ctx, socket_t lfd) {
  lx_listener_t *listener = malloc(sizeof(lx_listener_t));
  listener->fd = lfd;
  listener->data = NULL;
  listener->onaccept = NULL;

  listener->event.ctx = ctx;
  listener->event.flags = 0;
  listener->event.read = lx_listener_handler;
  listener->event.write = NULL;

  return listener;
}

/* epoll event handler for listener */
void lx_listener_handler(lx_event_t *event) {
  lx_listener_t *listener = container_of(event, lx_listener_t, event);

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

  lx_make_nonblocking(client_fd);

  lx_connection_t *conn = lx_connection_init(event->ctx, client_fd);
  conn->listener = listener;

  lx_add_event(&conn->event, client_fd);
  if (listener->onaccept != NULL)
    listener->onaccept(conn);
}

/* epoll event handler for connection (EPOLLIN) */
void lx_connection_read(lx_event_t *event) {
    lx_connection_t *conn = container_of(event, lx_connection_t, event);
    if (conn->ondata != NULL)
      conn->ondata(conn);
}

/* epoll event handler for connection (EPOLLOUT) */
void lx_connection_write(lx_event_t *event) {
  lx_connection_t *conn = container_of(event, lx_connection_t, event);

  // idk what to do in this scenario
  if (conn->output == NULL)
    return;

  size_t to_write = conn->output_size - conn->written;
  char *buf = conn->output + conn->written;

  ssize_t written = write(conn->fd, buf, to_write);
  if (written == -1)
    perror("socket::write");
  
  conn->written += written;

  if (conn->written == conn->output_size) {
    printf("Everything is written\n");
    conn->written = 0;
    conn->output_size = 0;
    conn->output = NULL;
    lx_stop_writing(event, conn->fd);
  }
}

lx_listener_t *lx_listen(lx_io_t *ctx, int port, void (*onaccept)(struct lx_connection *)) {
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

  lx_make_nonblocking(lfd);

  if (listen(lfd, SOMAXCONN) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  
  lx_listener_t *listener = lx_listener_init(ctx, lfd);
  listener->onaccept = onaccept;

  lx_add_event(&listener->event, lfd);
  return listener;
}

/* closes tcp connection, removes from epoll and free connection from memory */
void lx_close(lx_connection_t *conn) {
  lx_remove_event(&conn->event, conn->fd);
  close(conn->fd);
  if (conn->onclose)
    conn->onclose(conn); // user should clear data, close timers etc
  
  free(conn);
}

// TODO: add callback: void (*onwrite)(char *buf, size_t size)
int lx_write(lx_connection_t *conn, char *buf, size_t size) {
  /*
  int written = write(conn->fd, buf, size);

  if (written < 0) {
    if (written != EWOULDBLOCK || written != EAGAIN)
      return -1;
  }
  */
  conn->output = buf;
  conn->output_size = size;
  //conn->written += written;

  lx_set_write_event(&conn->event, conn->fd);
}
