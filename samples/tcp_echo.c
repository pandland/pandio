#include <pandio.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

void handle_close(pnd_tcp_t *stream) {
  // perform clean up
  printf("Connection closed: %d\n", stream->fd);
  free(stream);
}

void handle_write(pnd_write_t *write_op, int status) {
  printf("write completed\n");
  free(write_op->buf);
  free(write_op);
}

// echo server
void handle_read(pnd_tcp_t *stream) {
  char buf[1024] = {};
  ssize_t bytes = read(stream->fd, buf, 1024);
  if (bytes < 0) {
    perror("read");
    return;
  }

  if (bytes == 0) {
    //pnd_tcp_close(stream);
    printf("Received 0 bytes lol\n");
    pnd_tcp_destroy(stream);
    return;
  }

  printf("read %ld bytes\n", bytes); fflush(stdout);
  pnd_write_t *write_op = malloc(sizeof(pnd_write_t));
  pnd_tcp_write_init(write_op, malloc(bytes), bytes, handle_write);
  memcpy((void*)write_op->buf, (void*)buf, bytes);
  pnd_tcp_write_async(stream, write_op);
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
