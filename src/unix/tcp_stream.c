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

#include "tcp_stream.h"
#include <stdio.h>
#include "poll.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

void pnd_tcp_init(pnd_io_t *ctx, pnd_tcp_t *stream) 
{
  stream->fd = 0;
  stream->writes_size = 0;
  stream->state = PND_TCP_NONE;
  stream->data = NULL;
  stream->on_close = NULL;
  stream->on_data = NULL;
  queue_init(&stream->writes);
  pnd_init_event(&stream->ev);
  stream->ev.ctx = ctx;
}

int pnd_close_fd(pnd_fd_t fd)
{
  int status;
  do {
    status = close(fd);
  } while (status == -1 && errno == EINTR);

  return status;
}

/* handler for I/O events from epoll/kqueue */
void pnd_tcp_listener_io(struct pnd_event *event, unsigned events)
{
  assert(events & PND_READABLE);

  pnd_tcp_t *listener = container_of(event, pnd_tcp_t, ev);

  if (listener->state != PND_TCP_ACTIVE) {
    return;
  }

  struct sockaddr_in address;
  int addrlen = sizeof(address);
  pnd_fd_t peer_fd = accept(listener->fd, (struct sockaddr *)&address, &addrlen);
  if (peer_fd < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      // ignore error and wait for another pnd_tcp_listener_io call...
      return;
    } else {
      perror("accept");
      return;
    }
  }

  if (pnd_set_nonblocking(peer_fd) < 0) {
    pnd_close_fd(peer_fd);
    return;
  }

  if (listener->on_connect != NULL) {
    listener->on_connect(listener, peer_fd);
  } else {
    // no handler set, so close socket to avoid fd leak. 
    pnd_close_fd(peer_fd);
  }
}

int pnd_tcp_listen(pnd_tcp_t *server, int port, void (*onconnect)(pnd_tcp_t*, int)) 
{
  pnd_fd_t lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lfd <= 0) {
    perror("socket");
    return -1;
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  int opt = 1;

  if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    pnd_close_fd(lfd);
    return -1;
  }

  if (bind(lfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
    pnd_close_fd(lfd);
    return -1;
  }

  if (pnd_set_nonblocking(lfd) < 0) {
    pnd_close_fd(lfd);
    return -1;
  }

  if (listen(lfd, SOMAXCONN) < 0) {
    perror("listen");
    pnd_close_fd(lfd);
    return -1;
  }

  server->fd = lfd;
  server->ev.callback = pnd_tcp_listener_io;
  server->state = PND_TCP_ACTIVE;
  server->on_connect = onconnect;

  pnd_add_event(&server->ev, lfd);

  return 0;
}

/* Actually writes enqueued writes */
void pnd_tcp_write_io(pnd_tcp_t *stream) 
{
  // sometimes we get stale event when stream was closed in the same iteration
  if (stream->state == PND_TCP_CLOSED)
    return;

  while (!queue_empty(&stream->writes)) {
    struct queue_node *next = queue_peek(&stream->writes);
    pnd_write_t *write_op = container_of(next, pnd_write_t, qnode);
    
    size_t to_write = write_op->size - write_op->written;
    const char *buf = write_op->buf + write_op->written;
    ssize_t written;

    do {
      written = write(stream->fd, buf, to_write);
    } while (written < 0 && errno == EINTR);

    if (written < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return;
      } else {
        perror("write");
        queue_pop(&stream->writes);
        write_op->cb(write_op, -1);
        return;
      }
    }

    write_op->written += written;

    if (write_op->written == write_op->size) {
      queue_pop(&stream->writes);
      if (write_op->cb)
        write_op->cb(write_op, 0);
    }
  }

  if (queue_empty(&stream->writes)) {
    pnd_stop_writing(&stream->ev, stream->fd);
    
    if (stream->state == PND_TCP_CLOSING)
      shutdown(stream->fd, SHUT_WR);
  }
}

void pnd_tcp_write_init(pnd_write_t *write_op, char *buf, size_t size, write_cb_t cb)
{
  write_op->buf = buf;
  write_op->size = size;
  write_op->written = 0;
  write_op->cb = cb;
  write_op->data = NULL;
  queue_init_node(&write_op->qnode);
}

#define PND_ERROR -1
#define PND_EAGAIN -2

/* Try to write synchronously */
ssize_t pnd_tcp_try_write(pnd_tcp_t *stream, const char *chunk, size_t size)
{
  if (stream->state != PND_TCP_ACTIVE) {
    return PND_ERROR;
  }

  if (!queue_empty(&stream->writes))
    return PND_EAGAIN;

  ssize_t written;

  do {
    written = write(stream->fd, chunk, size);
  } while (written < 0 && errno == EINTR);

  if (written < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return PND_EAGAIN;
    } else {
      perror("write");
      return PND_ERROR;
    }
  }

  return written;
}

void pnd_tcp_write(pnd_tcp_t *stream, pnd_write_t *write_op)
{
  ssize_t written = pnd_tcp_try_write(stream, write_op->buf, write_op->size);
  if (written == PND_ERROR) {
    write_op->cb(write_op, -1);
    return;
  }

  if (written == PND_EAGAIN) {
    pnd_tcp_write_async(stream, write_op);
    return;
  }

  if (written < write_op->size) {
    write_op->written += written;
    pnd_tcp_write_async(stream, write_op);
  } else {
    write_op->cb(write_op, 0);
  }
}

/* Enqueue write operation and do it asynchronously.
 * Usually requires copying data to the new structure
 */
void pnd_tcp_write_async(pnd_tcp_t *stream, pnd_write_t *write_op)
{
  if (stream->state != PND_TCP_ACTIVE) {
    write_op->cb(write_op, -1);
    return;
  }

  queue_init_node(&write_op->qnode);
  queue_push(&stream->writes, &write_op->qnode);
}

/* handler for I/O events from epoll/kqueue */
void pnd_tcp_client_io(struct pnd_event * event, unsigned events)
{
  pnd_tcp_t *stream = container_of(event, pnd_tcp_t, ev);

  if (events & PND_CLOSE) {
    printf("Detected PND_CLOSE\n");
    pnd_tcp_destroy(stream);
    return;
  }

  if (events & PND_READABLE) {
    // TODO: actually read data and pass chunk to the on_data callback, instrad
    stream->on_data(stream);
  }

  if (events & PND_WRITABLE) {
    pnd_tcp_write_io(stream);
  }
}

int pnd_tcp_reject(pnd_fd_t fd) 
{
  return pnd_close_fd(fd);
}

void pnd_tcp_accept(pnd_tcp_t *peer, pnd_fd_t fd)
{
  pnd_set_nonblocking(fd);
  peer->fd = fd;
  peer->state = PND_TCP_ACTIVE;
  peer->ev.callback = pnd_tcp_client_io;
  pnd_add_event(&peer->ev, fd);
}

void pnd_tcp_pause(pnd_tcp_t *stream)
{
  pnd_stop_reading(&stream->ev, stream->fd);
}

void pnd_tcp_resume(pnd_tcp_t *stream)
{
  pnd_stop_reading(&stream->ev, stream->fd);
}

/* forcefully closes tcp stream and discards all enqueued writes */
void pnd_tcp_destroy(pnd_tcp_t *stream)
{
  if (stream->state == PND_TCP_CLOSED)
    return;

  stream->state = PND_TCP_CLOSED;
  pnd_remove_event(&stream->ev, stream->fd);
  pnd_close_fd(stream->fd);

  while (!queue_empty(&stream->writes)) {
    struct queue_node *next = queue_pop(&stream->writes);
    pnd_write_t *write_op = container_of(next, pnd_write_t, qnode);
    write_op->cb(write_op, -1);
  }

  queue_init_node(&stream->close_qnode);
  queue_push(&stream->ev.ctx->pending_closes, &stream->close_qnode);
}

/* gracefully closes tcp stream */
void pnd_tcp_close(pnd_tcp_t *stream) 
{
  if (stream->state != PND_TCP_ACTIVE)
    return;

  stream->state = PND_TCP_CLOSING;

  if (queue_empty(&stream->writes)) {
    /* call shutdown and continue to read until EOF */
    shutdown(stream->fd, SHUT_WR);
  } else {
    // wait for all writes to finish
    pnd_start_writing(&stream->ev, stream->fd);
  }
}
