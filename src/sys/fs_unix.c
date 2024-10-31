#include "pandio/err.h"
#include "pandio/fs.h"
#include "pandio/threadpool.h"
#include "string.h"
#include <fcntl.h>
#include <unistd.h>

void pd__fs_work_done(pd_task_t *task) {
  pd_fs_t *op = (pd_fs_t*)(task);
  op->cb(op);
}

void pd_fs_init(pd_io_t *ctx, pd_fs_t *op) {
  memset(op, 0, sizeof(pd_fs_t));
  op->ctx = ctx;
  op->status = PD_UNKNOWN;
  op->type = pd_unknown_op;
  op->task.done = pd__fs_work_done;
}

void pd__fs_open_work(pd_task_t *task) {
  pd_fs_t *op = (pd_fs_t*)(task);
  char *path = op->params.open.path;
  int fd;

  do {
    fd = open(path, O_RDONLY); // TODO: make flags configurable
  } while (fd < 0 && errno == EINTR);

  if (fd < 0) {
    op->status = pd_errno();
  } else {
    op->status = 0;
  }

  op->result.fd = fd;
  free(path); 
}

void pd_fs_open(pd_fs_t *op, const char *path, void (*cb)(pd_fs_t *)) {
  op->params.open.path = strdup(path);
  op->type = pd_open_op;
  op->cb = cb;
  op->task.work = pd__fs_open_work;

  pd_task_submit(op->ctx, &op->task);
}
