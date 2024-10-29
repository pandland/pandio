#pragma once
#include "pandio/core.h"
#include "pandio/threadpool.h"

#define FS_OPS(X)                                                              \
  X(open, pd_fd_t fd; char *path;)                                             \
  X(read, pd_fd_t fd; char *buf; size_t size;)

#define DEFINE_STRUCT(name, fields)                                            \
  typedef struct pd_fs_##name##_op {                                           \
    pd_task_t inner;                                                           \
    int status;                                                                \
    void (*cb)(struct pd_fs_##name##_op *);                                    \
    fields                                                                     \
  } pd_fs_##name##_t;

FS_OPS(DEFINE_STRUCT)

#define DEFINE_TASK_DONE(name, fields)                                         \
  static void pd__fs_##name##_done(pd_task_t *task) {                          \
    pd_fs_##name##_t *op = (pd_fs_##name##_t *)(task);                         \
    op->cb(op);                                                                \
    free(op);                                                                  \
  }

FS_OPS(DEFINE_TASK_DONE)

void pd_fs_open(pd_io_t *ctx, const char *path, void (*cb)(pd_fs_open_t *));

void pd_fs_read(pd_io_t *ctx, pd_fd_t fd, char *buf, size_t size,
                void (*cb)(pd_fs_read_t *));
