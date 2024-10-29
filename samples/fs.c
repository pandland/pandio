#include "pandio/core.h"
#include "pandio/threadpool.h"
#include <pandio.h>
#include <stdio.h>

void callback(int status, pd_fd_t fd) {
  if (status < 0) {
    printf("error: %s\n", pd_errstr(status));
    return;
  }

  printf("fd is: %d\n", fd); 
}

int main() {
  // it's import to allocate pd_io_t on the heap when using threadpool...
  pd_io_t *ctx = malloc(sizeof(pd_io_t));
  pd_io_init(ctx);
  pd_threadpool_init(4);

  pd_fs_open(ctx, ".clang-format", callback);

  pd_io_run(ctx);
  pd_threadpool_end();
}
