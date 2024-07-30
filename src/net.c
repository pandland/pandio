#include "net.h"
#include "common.h"

lx_connection_t *lx_connection_init(lx_io_t *ctx, socket_t fd) {
  lx_connection_t *conn = malloc(sizeof(lx_connection_t));
  conn->fd = fd;
  conn->ondata = NULL;
  conn->onclose = NULL;
  conn->data = NULL;
  conn->size = 0;
  conn->closing = 0;

  queue_init(&conn->output);

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
    perror("socket->accept");
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

  while (!queue_empty(&conn->output)) {
    struct queue_node *next = conn->output.head;
    lx_write_t *write_op = container_of(next, lx_write_t, qnode);

    size_t to_write = write_op->size - write_op->written;
    const char *buf = write_op->buf + write_op->written;
    ssize_t written = write(conn->fd, buf, to_write);

    if (written < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return;

      perror("socket->write");
      queue_pop(&conn->output);
      write_op->cb(write_op, -1);
      return;
    }

    write_op->written += written;

    if (write_op->written == write_op->size) {
      queue_pop(&conn->output);
      if (write_op->cb)
        write_op->cb(write_op, 0);
    }
  }

  if (queue_empty(&conn->output)) {
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

/* closes tcp connection, removes from epoll and frees connection from memory */
void lx_close(lx_connection_t *conn) {
  if (conn->closing) {
    return;
  }
  
  conn->closing = 1;
  lx_remove_event(&conn->event, conn->fd);
  close(conn->fd);

  while (!queue_empty(&conn->output)) {
    struct queue_node *next = queue_pop(&conn->output);
    lx_write_t *write_op = container_of(next, lx_write_t, qnode);
    // call callback on pending writes when closing
    write_op->cb(write_op, -1);
  }
  
  // enqueue connection to close in next cycle
  queue_init_node(&conn->close_qnode);
  queue_push(&conn->event.ctx->pending_closes, &conn->close_qnode);
}

void lx_close_pending(lx_io_t *ctx) {
  while (!queue_empty(&ctx->pending_closes)) {
    struct queue_node *next = queue_pop(&ctx->pending_closes);
    lx_connection_t *conn = container_of(next, lx_connection_t, close_qnode);

    if (conn->onclose != NULL)
      conn->onclose(conn);

    free(conn);
  }
}

lx_write_t *lx_write_alloc(const char *buf, size_t size) {
  lx_write_t *write_op = malloc(sizeof(lx_write_t));
  write_op->buf = buf;
  write_op->size = size;
  write_op->written = 0;
  write_op->data = NULL;

  return write_op;
}

void enqueue_write(lx_write_t *write_op, lx_connection_t *conn) {
  queue_init_node(&write_op->qnode);
  queue_push(&conn->output, &write_op->qnode);
}

int lx_write(lx_write_t *write_op, lx_connection_t *conn, write_cb_t cb) {
  write_op->cb = cb;

  if (queue_empty(&conn->output)) {
    // try write
    ssize_t written = write(conn->fd, write_op->buf, write_op->size);
    if (written < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        enqueue_write(write_op, conn);
        lx_set_write_event(&conn->event, conn->fd);
        return 0;
      } else {
        perror("socket->write");
        write_op->cb(write_op, -1);
        return -1;
      }
    }

    // fully written
    if (written == write_op->size) {
      if (cb != NULL)
        cb(write_op, 0);
      return 1;
    }

    // partial write:
    write_op->written = written;
    enqueue_write(write_op, conn);
    lx_set_write_event(&conn->event, conn->fd);

    return 0;
  }

  enqueue_write(write_op, conn);
  lx_set_write_event(&conn->event, conn->fd);

  return 0;
}
