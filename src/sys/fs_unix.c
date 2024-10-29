#include "pandio/core.h"
#include "pandio/fs.h"
#include "pandio/threadpool.h"
#include "string.h"
#include <fcntl.h>
#include <unistd.h>

void pd__fs_open(pd_task_t *task) {
  pd_fs_open_t *op = (pd_fs_open_t *)(task);

  do {
    op->fd = open(op->path, O_RDONLY); // TODO: make flags configurable
  } while (op->fd < 0 && errno == EINTR);

  if (op->fd < 0) {
    op->status = pd_errno();
  }

  free(op->path);
}

void pd_fs_open(pd_io_t *ctx, const char *path, void (*cb)(pd_fs_open_t *)) {
  pd_fs_open_t *op = malloc(sizeof(pd_fs_open_t));
  op->path = strdup(path);
  op->cb = cb;
  op->status = 0;
  op->inner.work = pd__fs_open;
  op->inner.done = pd__fs_open_done;

  pd_task_submit(ctx, &op->inner);
}


void pd__fs_read(pd_task_t *task) {
  pd_fs_read_t *op = (pd_fs_read_t *)(task);
  ssize_t nread;

  do {
    nread = read(op->fd, op->buf, op->size);
  } while (nread < 0 && errno == EINTR);

  if (nread < 0) {
    op->status = pd_errno();
    return;
  }

  op->size = nread;
}

void pd_fs_read(pd_io_t *ctx, pd_fd_t fd, char *buf, size_t size,
                void (*cb)(pd_fs_read_t *)) {
  pd_fs_read_t *op = malloc(sizeof(pd_fs_read_t));
  op->fd = fd;
  op->buf = buf;
  op->size = size;
  op->cb = cb;
  op->status = 0;
  op->inner.work = pd__fs_read;
  op->inner.done = pd__fs_read_done;

  pd_task_submit(ctx, &op->inner);
}
