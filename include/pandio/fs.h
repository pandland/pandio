#pragma once
#include "pandio/core.h"
#include "pandio/threadpool.h"

#define FS_OPS(X)                                                              \
  X(open, { char *path; }, pd_fd_t fd)                                         \
  X(read, { pd_fd_t fd; char *buf; size_t size; }, ssize_t nread)              \
  X(write, { pd_fd_t fd; char *buf; size_t size; }, ssize_t nwritten)          \
  X(close, { pd_fd_t fd; char *buf; size_t size; },)                           \

#define FS_TYPES(type, params, result) \
  pd_##type##_op,

enum pd_fs_type {
  FS_OPS(FS_TYPES)
  pd_unknown_op = -1
};

typedef enum pd_fs_type pd_fs_type_t ;

#define FS_PARAMS(type, params, result) \
  struct params type; \

#define FS_RESULTS(type, params, result) \
  result; \

struct pd_fs_s {
  pd_task_t task; // must be first, because we later cast from it
  pd_io_t *ctx;
  pd_fs_type_t type;
  void (*cb)(struct pd_fs_s*);
  int status;
  union {
    FS_OPS(FS_PARAMS)
  } params;
  union {
    FS_OPS(FS_RESULTS)
  } result;
};

typedef struct pd_fs_s pd_fs_t;

void pd_fs_init(pd_io_t *ctx, pd_fs_t *op);

void pd_fs_open(pd_fs_t *op, const char *path, void (*cb)(pd_fs_t *));

/*
void pd_fs_read(pd_fs_t *op, pd_fd_t fd, char *buf, size_t size,
                void (*cb)(pd_fs_t *));
*/
