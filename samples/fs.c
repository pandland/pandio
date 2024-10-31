#include <pandio.h>
#include <stdio.h>

void on_read(pd_fs_t *op) {
  if (op->status < 0) {
    printf("error: %s\n", pd_errstr(op->status));
    goto cleanup;
  }

  char *buf = op->params.read.buf;
  int size = op->result.nread;
  printf("%.*s\n", size, buf);
  free(buf);

cleanup:
  free(op);
}

void on_open(pd_fs_t *op) {
  if (op->status < 0) {
    printf("error: %s\n", pd_errstr(op->status));
    goto cleanup;
  }

  int fd = op->result.fd;
  printf("fd is: %d\n", fd);

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
