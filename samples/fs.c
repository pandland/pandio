#include "pandio/fs.h"
#include <pandio.h>
#include <stdio.h>

void on_read(pd_fs_read_t *op) {
  if (op->status < 0) {
    printf("error: %s\n", pd_errstr(op->status));
    return;
  }

  printf("%.*s\n", (int)op->size, op->buf);
}

void on_open(pd_fs_open_t *op) {
  if (op->status < 0) {
    printf("error: %s\n", pd_errstr(op->status));
    return;
  }
  printf("fd is: %d\n", op->fd);

  size_t size = 16 * 1024;
  char *buf = malloc(size);
  pd_fs_read(op->inner.ctx, op->fd, buf, size, on_read);
}

int main() {
  // it's import to allocate pd_io_t on the heap when using threadpool...
  pd_io_t *ctx = malloc(sizeof(pd_io_t));
  pd_io_init(ctx);
  pd_threadpool_init(4);

  pd_fs_open(ctx, "cmake_install.cmake", on_open);

  pd_io_run(ctx);
  pd_threadpool_end();
}
