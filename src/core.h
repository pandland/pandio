#pragma once

#include <stdint.h>
#include "heap.h"

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
typedef HANDLE pd_fd_t;
#else
typedef int pd_fd_t;
#endif

struct pd_io_s {
    pd_fd_t poll_fd;
    uint64_t now;
    struct heap timers;
};

typedef struct pd_io_s pd_io_t;

void pd_io_init(pd_io_t*);

void pd_io_run(pd_io_t*);

struct pd_event_s {
#ifdef _WIN32
    /* must be a first member, because we will cast OVERLAPPED to the pd_event_t */
    OVERLAPPED overlapped;
    size_t bytes;   // bytes transferred
#endif
    void (*handler)(struct pd_event_s*);
    void *data;
    unsigned flags;
};

typedef struct pd_event_s pd_event_t;

void pd_event_init(pd_event_t*);
