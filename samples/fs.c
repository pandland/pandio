#include "pandio/core.h"
#include "pandio/threadpool.h"
#include <pandio.h>
#include <stdio.h>

void callback(pd_fd_t fd) { printf("fd is: %d\n", fd); }

int main() {
  pd_io_t *ctx = malloc(sizeof(pd_io_t));
  pd_io_init(ctx);
  pd_threadpool_init(4);

  pd_fs_open(ctx, "Makefile", callback);

  pd_io_run(ctx);
  pd_threadpool_end();
}
