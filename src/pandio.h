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

struct pnd_io {
  pnd_fd_t poll_handle;
  size_t handles;
  uint64_t now;
  struct heap timers;
  struct queue pending_closes;
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

// exports
#include "tcp_stream.h"
#include "timers.h"

void pnd_io_init(pnd_io_t *ctx);

void pnd_io_run(pnd_io_t *ctx);
