#include "internal.h"
#include "pandio/core.h"
#include "pandio/threadpool.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct pd__fs_open {
  void (*cb)(int, pd_fd_t);
  char *path;
  pd_fd_t fd;
  int status;
  pd_task_t inner;
};

typedef struct pd__fs_open pd__fs_open_t;

void pd__fs_open_work(pd_task_t *task) {
  pd__fs_open_t *work = container_of(task, pd__fs_open_t, inner);
  work->fd = open(work->path, O_RDONLY);
  if (work->fd < 0) {
    work->status = pd_errno(); // I think errno is "thread-local", so need to
                               // pass errno status to main thread
  } else {
    work->status = 0;
  }

  free(work->path);
}

void pd__fs_open_done(pd_task_t *task) {
  pd__fs_open_t *work = container_of(task, pd__fs_open_t, inner);
  work->cb(work->status, work->fd);
  work->inner.ctx->refs--;
  free(work);
}

void pd_fs_open(pd_io_t *ctx, const char *path, void (*cb)(int, pd_fd_t)) {
  pd__fs_open_t *task = malloc(sizeof(pd__fs_open_t));
  task->inner.work = pd__fs_open_work;
  task->inner.done = pd__fs_open_done;
  task->inner.udata = NULL;
  task->cb = cb;
  task->path = strdup(path);
  ctx->refs++;

  pd_task_submit(ctx, &task->inner);
}
