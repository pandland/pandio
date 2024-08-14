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

#pragma once
#include "pandio.h"

#define pnd_tcp_ctx(pnd_tcp) ((pnd_tcp)->ev.ctx)

enum {
  PND_TCP_NONE = 0,  // initial state, when socket is not initialized yet but zeroed struct exists.
  PND_TCP_ACTIVE,
  PND_TCP_CLOSING,
  PND_TCP_CLOSED
};

struct pnd_tcp {
  pnd_fd_t fd;
  pnd_event_t ev;
  int state;
  struct queue writes;
  size_t writes_size;
  struct queue_node close_qnode; // used to queue tcp streams for closing
  // public fields for user to use:
  void *data;
  void (*on_data)(struct pnd_tcp *);
  void (*on_close)(struct pnd_tcp *);
  void (*on_connect)(struct pnd_tcp*, pnd_fd_t);
};

typedef struct pnd_tcp pnd_tcp_t;

struct pnd_write;

typedef void (*write_cb_t)(struct pnd_write*, int status);

struct pnd_write {
  char *buf;
  size_t size;
  size_t written;
  write_cb_t cb;
  void *data;
  struct queue_node qnode;
};

typedef struct pnd_write pnd_write_t;

void pnd_tcp_init(pnd_io_t *ctx, pnd_tcp_t *stream);

int pnd_tcp_listen(pnd_tcp_t *server, int port, void (*onconnect)(pnd_tcp_t*, int));

void pnd_tcp_write_init(pnd_write_t *write_op, char *buf, size_t size, write_cb_t cb);

#define PND_ERROR -1
#define PND_EAGAIN -2

/* Try to write synchronously */
ssize_t pnd_tcp_try_write(pnd_tcp_t *stream, const char *chunk, size_t size);

void pnd_tcp_write(pnd_tcp_t *stream, pnd_write_t *write_op);

/* Enqueue write operation and do it asynchronously.
 * Usually requires copying data to the new structure
 */
void pnd_tcp_write_async(pnd_tcp_t *stream, pnd_write_t *write_op);
int pnd_tcp_reject(pnd_fd_t fd);

void pnd_tcp_accept(pnd_tcp_t *peer, pnd_fd_t fd);

void pnd_tcp_pause(pnd_tcp_t *stream);

void pnd_tcp_resume(pnd_tcp_t *stream);

/* forcefully closes tcp stream and discards all enqueued writes */
void pnd_tcp_destroy(pnd_tcp_t *stream);
/* gracefully closes tcp stream */
void pnd_tcp_close(pnd_tcp_t *stream);
