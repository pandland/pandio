#include "pandio/fs.h"
#include <pandio.h>
#include <stdio.h>

void on_close(pd_fs_t *op) {
  if (op->status < 0) {
    printf("\nerror(close): %s\n", pd_errstr(op->status));
  } else {
    printf("\nFile %d closed.\n", op->params.close.fd);
  }

  free(op);
}

void on_read(pd_fs_t *op) {
  char *buf = op->params.read.buf;
  if (op->status < 0) {
    printf("error(read): %s\n", pd_errstr(op->status));
    goto cleanup;
  }

  int size = op->result.nread;
  printf("%.*s\n", size, buf);
  int fd = op->params.read.fd;
  pd_fs_init(op->ctx, op);
  //close(fd);  // for testing error handling inside on_close
  pd_fs_close(op, fd, on_close);
  free(buf);
  return;

cleanup:
  free(buf);
  free(op);
}

void on_open(pd_fs_t *op) {
  if (op->status < 0) {
    printf("error(open): %s\n", pd_errstr(op->status));
    goto cleanup;
  }

  int fd = op->result.fd;
  printf("fd is: %d\n", fd);
  //close(fd);  // for testing error handling inside on_read
  pd_fs_init(op->ctx, op);
  size_t size = 16 * 1024;
  char *buf = malloc(size);
  pd_fs_read(op, fd, buf, size, on_read);
  return;

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
