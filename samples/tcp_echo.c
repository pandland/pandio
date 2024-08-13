#include <pandio.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

void handle_close(pnd_tcp_t *stream) {
  // perform clean up
  //free(stream);
  printf("Connection closed\n");
}

// echo server
void handle_read(pnd_tcp_t *stream) {
  char buf[1024] = {};
  ssize_t bytes = read(stream->fd, buf, 1024);
  printf("read %ld bytes\n", bytes); fflush(stdout);
  pnd_tcp_try_write(stream, buf, bytes);
  printf("tried to write :*\n"); fflush(stdout);
  pnd_tcp_close(stream);
}

void handle_connection(pnd_tcp_t *server, pnd_fd_t fd) {
  printf("New connection accepted: %d\n", fd);
  pnd_tcp_t *client = malloc(sizeof(pnd_tcp_t));
  pnd_io_t *ctx = pnd_tcp_ctx(server);
  pnd_tcp_init(ctx, client);
  pnd_tcp_accept(client, fd);

  client->on_data = handle_read;
  client->on_close = handle_close;
}

int main() {
  pnd_io_t ctx;
  pnd_io_init(&ctx);

  pnd_tcp_t *server = malloc(sizeof(pnd_tcp_t));
  pnd_tcp_init(&ctx, server);
  pnd_tcp_listen(server, 8000, handle_connection);

  pnd_io_run(&ctx);
  
  return 0;
}
