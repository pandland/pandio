#include "core.h"
#include "timers.h"
#include <fcntl.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


uint64_t pd_now() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}


int pd_set_nonblocking(pd_fd_t fd) {
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

#define MAX_EVENTS 1024

void pd_io_run(pd_io_t *ctx) {
    struct kevent events[MAX_EVENTS];
    struct kevent ev;
    int timeout = pd_timers_next(ctx);
    pd_event_t *pev;

    // TODO: break loop if there is no active handles
    while (true) {
        struct timespec ts;
        if (timeout >= 0) {
            ts.tv_sec = timeout / 1000;
            ts.tv_nsec = (timeout % 1000) * 1000000;
        }

        int nevents = kevent(ctx->poll_fd, NULL, 0, events, MAX_EVENTS, timeout >= 0 ? &ts : NULL);

        if (nevents == -1) {
            perror("kevent");
            exit(EXIT_FAILURE);
        }

        ctx->now = pd_now();

        for (int i = 0; i < nevents; ++i) {
            ev = events[i];
            unsigned pevents = 0;
            pev = (pd_event_t *)ev.udata;

            if (ev.filter == EVFILT_READ)
                pevents |= PD_POLLIN;

            if (ev.filter == EVFILT_WRITE)
                pevents |= PD_POLLOUT;

            if ((ev.flags & EV_ERROR) || (ev.flags & EV_EOF)) {
                // usually we have to close handle in this scenario, so let's simplify detection
                pevents |= PD_CLOSE;

                if (ev.flags & EV_ERROR)
                    pevents |= PD_POLLERR;

                if (ev.flags & EV_EOF)
                    pevents |= PD_POLLHUP;
            }

            pev->handler(pev, pevents);
        }

        pd_timers_run(ctx);
        timeout = pd_timers_next(ctx);
    }
}
