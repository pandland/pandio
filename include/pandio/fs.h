#pragma once
#include "pandio/core.h"
#include "pandio/threadpool.h"

#define FS_OPS(X)                                                              \
  X(open, { char *path; })                                                     \
  X(read, {                                                                    \
    pd_fd_t fd;                                                                \
    char *buf;                                                                 \
    size_t size;                                                               \
  })                                                                           \
  X(write, {                                                                   \
    pd_fd_t fd;                                                                \
    char *buf;                                                                 \
    size_t size;                                                               \
  })                                                                           \
  X(close, {                                                                   \
    pd_fd_t fd;                                                                \
    char *buf;                                                                 \
    size_t size;                                                               \
  })

#define FS_TYPES(type, params) pd_##type##_op,

enum pd_fs_type { FS_OPS(FS_TYPES) pd_unknown_op = -1 };

typedef enum pd_fs_type pd_fs_type_t;

#define FS_PARAMS(type, params) struct params type;

struct pd_fs_s {
  pd_task_t task; // must be first, because later we cast from it
  pd_io_t *ctx;
  pd_fs_type_t type;
  void (*cb)(struct pd_fs_s *);
  int status;
  union {
    FS_OPS(FS_PARAMS)
  } params;
  union {
    pd_fd_t fd;
    ssize_t size;
  } result;
};

typedef struct pd_fs_s pd_fs_t;

typedef void (*pd_fs_cb_t)(pd_fs_t *);

void pd_fs_init(pd_io_t *, pd_fs_t *);

void pd_fs_open(pd_fs_t *, const char *, pd_fs_cb_t);

void pd_fs_read(pd_fs_t *, pd_fd_t, char *, size_t, pd_fs_cb_t);

void pd_fs_close(pd_fs_t *, pd_fd_t, pd_fs_cb_t);
