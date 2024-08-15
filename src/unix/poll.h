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

#include "pandio.h"

void pnd_poll_init(pnd_io_t * ctx);

void pnd_poll_run(pnd_io_t * ctx, int timeout);

void pnd_init_event(pnd_event_t * event);

void pnd_modify_event(pnd_event_t * event, int fd, uint32_t operation, uint32_t flags);

void pnd_add_event_readable(pnd_event_t *event, pnd_fd_t fd);

void pnd_add_event_writable(pnd_event_t *event, pnd_fd_t fd);

void pnd_start_reading(pnd_event_t * event, pnd_fd_t fd);

void pnd_stop_reading(pnd_event_t * event, pnd_fd_t fd);

void pnd_start_writing(pnd_event_t * event, pnd_fd_t fd);

void pnd_stop_writing(pnd_event_t * event, pnd_fd_t fd);

void pnd_remove_event(pnd_event_t * event, pnd_fd_t fd);

int pnd_set_nonblocking(pnd_fd_t fd);
