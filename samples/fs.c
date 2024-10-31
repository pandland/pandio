#include "pandio/fs.h"
#include <pandio.h>
#include <stdio.h>

void on_open(pd_fs_t *op) {
  if (op->status < 0) {
    printf("error: %s\n", pd_errstr(op->status));
    goto cleanup;
  }

  printf("fd is: %d\n", op->result.fd);

cleanup:
  free(op);
}

int main() {
  // it's import to allocate pd_io_t on the heap when using threadpool...
  pd_io_t *ctx = malloc(sizeof(pd_io_t));
  pd_io_init(ctx);
  pd_threadpool_init(4);

  pd_fs_t *op = malloc(sizeof(pd_fs_t));
  pd_fs_init(ctx, op);
  pd_fs_open(op, "Makefile", on_open);

  pd_io_run(ctx);
  pd_threadpool_end();
}
