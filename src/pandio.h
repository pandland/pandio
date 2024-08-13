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
#include <stdint.h>
#include "common.h"
#include "heap.h"
#include "queue.h"

#if defined(__linux__)
typedef int pnd_fd_t;
#endif

struct pnd_io {
  pnd_fd_t poll_handle;
  size_t handles;
  uint64_t now;
  struct heap timers;
};

typedef struct pnd_io pnd_io_t;

enum {
  PND_READABLE = 1,
  PND_WRITABLE = 2,
  PND_CLOSE = 4
};

struct pnd_event {
  unsigned flags;
  pnd_io_t *ctx;
  void (*callback)(struct pnd_event*, unsigned events);
};

typedef struct pnd_event pnd_event_t;

enum {
  PND_TCP_NONE = 0,  // initial state, when socket is not initialized yet but zeroed struct exists.
  PND_TCP_ACTIVE,
  PND_TCP_CLOSING,
  PND_TCP_CLOSED
};

struct pnd_tcp {
  pnd_fd_t fd;
  pnd_event_t ev;
  void (*io_handler)(struct pnd_event*, int events);
  int state;
  struct queue writes;
  size_t writes_size;
  // public fields for user to use:
  void *data;
  void (*on_data)(struct pnd_tcp *);
  void (*on_close)(struct pnd_tcp *);
  void (*on_connect)(pnd_fd_t);
};

typedef struct pnd_tcp pnd_tcp_t;

struct pnd_write;

typedef void (*write_cb_t)(struct pnd_write*, int status);

struct pnd_write {
  const char *buf;
  size_t size;
  size_t written;
  write_cb_t cb;
  void *data;
  struct queue_node qnode;
};

typedef struct pnd_write pnd_write_t;
