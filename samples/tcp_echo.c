#include <lib.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

void handle_close(pnd_tcp_t *stream) {
  // perform clean up
  free(stream);
}

// echo server
void handle_read(pnd_tcp_t *stream) {
  char buf[1024] = {};
  ssize_t bytes = read(stream->fd, buf, 1024);

  pnd_tcp_try_write(stream, buf, bytes);
  pnd_tcp_close(stream);
}

void handle_connection(pnd_fd_t fd) {
  printf("New connection accepted: %d\n", fd);
  pnd_tcp_t *client = malloc(sizeof(pnd_tcp_t));
  pnd_tcp_accept(client, fd);

  client->on_data = handle_read;
  client->on_close = handle_close;
}

int main() {
  pnd_tcp_t *server = malloc(sizeof(pnd_tcp_t));
  pnd_tcp_init(server);
  pnd_tcp_listen(server, 8080, handle_connection);
  
  return 0;
}
