#pragma once

#include <stdint.h>
#include "heap.h"

#ifdef _WIN32
#include <windows.h>
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
