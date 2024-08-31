#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "heap.h"
#include "queue.h"

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
typedef HANDLE pd_fd_t;

typedef CRITICAL_SECTION pd_mutex_t;
typedef CONDITION_VARIABLE pd_cond_t;
typedef HANDLE pd_thread_t;

#else
#include <pthread.h>
#include <errno.h>

typedef int pd_fd_t;

typedef pthread_mutex_t pd_mutex_t;
typedef pthread_cond_t pd_cond_t;
typedef pthread_t pd_thread_t;
#endif

void pd_sleep(unsigned);

void pd_mutex_init(pd_mutex_t*);

void pd_mutex_lock(pd_mutex_t*);

void pd_mutex_unlock(pd_mutex_t*);

void pd_mutex_destroy(pd_mutex_t*);

void pd_cond_init(pd_cond_t*);

void pd_cond_wait(pd_cond_t*, pd_mutex_t*);

void pd_cond_signal(pd_cond_t*);

void pd_cond_broadcast(pd_cond_t*);

void pd_cond_destroy(pd_cond_t*);

void pd_thread_create(pd_thread_t*, void* (*)(void*), void *);

void pd_thread_join(pd_thread_t*);

struct pd_notifier_s;

struct pd_io_s {
    pd_fd_t poll_fd;
    uint64_t now;
    struct heap timers;
    struct queue pending_closes;
    struct queue finished_tasks;
    struct pd_notifier_s *task_signal;
    bool task_signaled;
};

typedef struct pd_io_s pd_io_t;

enum pd_event_flags {
    PD_POLLIN = 1 << 0,
    PD_POLLOUT = 1 << 1,
    PD_POLLHUP = 1 << 2,
    PD_POLLRDHUP = 1 << 3,
    PD_POLLERR = 1 << 4,
    PD_CLOSE = 1 << 5
};

struct pd_event_s {
#ifdef _WIN32
    /* must be a first member, because we will cast OVERLAPPED to the pd_event_t */
    OVERLAPPED overlapped;
    size_t bytes;   // bytes transferred
    void (*handler)(struct pd_event_s*);
#else
    unsigned flags;
    void (*handler)(struct pd_event_s*, unsigned);
#endif
    void *data;
};

typedef struct pd_event_s pd_event_t;


void pd_io_init(pd_io_t*);

void pd_io_run(pd_io_t*);

struct pd_notifier_s {
    pd_io_t *ctx;
#ifdef __linux__
    pd_fd_t fd;  // on Linux we use eventfd
#else
    pd_fd_t fd[2]; // on BSD/macOS we use self-pipe
#endif
    void (*handler)(struct pd_notifier_s*);
    void *udata;
    pd_event_t event;
};
typedef struct pd_notifier_s pd_notifier_t;

void pd_notifier_init(pd_io_t*, pd_notifier_t*);

void pd_notifier_send(pd_notifier_t*);