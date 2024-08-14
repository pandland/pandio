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
#include "poll.h"
#include "common.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#define MAX_EVENTS 1024

uint64_t pnd_now()
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

void pnd_poll_init(pnd_io_t *ctx)
{
	ctx->poll_handle = epoll_create1(0);

	if (ctx->poll_handle == -1) {
		perror("epoll");
		exit(EXIT_FAILURE);
	}
}

void pnd_poll_run(pnd_io_t * ctx, int timeout)
{
	struct epoll_event events[MAX_EVENTS];
	struct epoll_event ev;
	pnd_event_t *pev;
	int pevents;

	int events_count =
	    epoll_wait(ctx->poll_handle, events, MAX_EVENTS, timeout);
	if (events_count == -1) {
		perror("epoll_wait");
		exit(EXIT_FAILURE);
	}

	ctx->now = pnd_now();

	for (int i = 0; i < events_count; ++i) {
		pevents = 0;
		ev = events[i];
		pev = ev.data.ptr;

		if (ev.events & EPOLLIN) {
			pevents |= PND_READABLE;
		}

		if (ev.events & EPOLLOUT)
			pevents |= PND_WRITABLE;

		if (ev.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
			pevents |= PND_CLOSE;
		}

		pev->callback(pev, pevents);
	}
}

void pnd_init_event(pnd_event_t *event)
{
	event->callback = NULL;
	event->ctx = NULL;
	event->flags = 0;
}

void pnd_modify_event(pnd_event_t *event, int fd, uint32_t operation,
		      uint32_t flags)
{
	struct epoll_event ev;
	event->flags = flags;

	ev.events = event->flags;
	ev.data.ptr = event;
	if (epoll_ctl(event->ctx->poll_handle, operation, fd, &ev) == -1) {
		perror("pnd_modify_event");
	}
}

void pnd_add_event(pnd_event_t * event, pnd_fd_t fd)
{
	printf("FD: %d\n", fd);
	event->flags |= EPOLLIN;
	event->ctx->handles++;
	pnd_modify_event(event, fd, EPOLL_CTL_ADD, event->flags);
}

void pnd_start_reading(pnd_event_t * event, pnd_fd_t fd)
{
	event->flags |= EPOLLIN;
	pnd_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void pnd_stop_reading(pnd_event_t * event, pnd_fd_t fd)
{
	event->flags &= ~EPOLLIN;
	pnd_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void pnd_start_writing(pnd_event_t * event, pnd_fd_t fd)
{
	event->flags |= EPOLLOUT;
	pnd_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void pnd_stop_writing(pnd_event_t * event, pnd_fd_t fd)
{
	event->flags &= ~EPOLLOUT;
	pnd_modify_event(event, fd, EPOLL_CTL_MOD, event->flags);
}

void pnd_remove_event(pnd_event_t * event, pnd_fd_t fd)
{
	event->ctx->handles--;
	if (epoll_ctl(event->ctx->poll_handle, EPOLL_CTL_DEL, fd, NULL) == -1) {
		perror("pnd_remove_event");
	}
}

int pnd_set_nonblocking(pnd_fd_t fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1) {
		perror("fcntl");
		return -1;
	}

	return 0;
}
