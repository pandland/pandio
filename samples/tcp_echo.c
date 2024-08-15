#include <pandio.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

void handle_close(pnd_tcp_t *stream) {
  // perform clean up
  //printf("Connection closed: %d\n", stream->fd);
  free(stream);
}

void handle_write(pnd_write_t *write_op, int status) {
  //printf("write completed with status: %d\n", status);
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
    printf("Received 0 bytes for: %d\n", stream->fd);
    pnd_tcp_destroy(stream);
    return;
  }

  //printf("read %ld bytes\n", bytes); fflush(stdout);
  pnd_write_t *write_op = malloc(sizeof(pnd_write_t));
  const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
  pnd_tcp_write_init(write_op, strdup(response), strlen(response), handle_write);
  //memcpy((void*)write_op->buf, (void*)buf, bytes);
  pnd_tcp_write_async(stream, write_op);
  //pnd_tcp_try_write(stream, response, strlen(response));
  pnd_tcp_close(stream);
}

void handle_connection(pnd_tcp_t *server, pnd_fd_t fd) {
  //printf("New connection accepted: %d\n", fd);
  pnd_tcp_t *client = malloc(sizeof(pnd_tcp_t));
  pnd_io_t *ctx = pnd_tcp_ctx(server);
  pnd_tcp_init(ctx, client);
  pnd_tcp_accept(client, fd);

  client->on_data = handle_read;
  client->on_close = handle_close;
}

int main_old() {
  printf("Starting server, pid is: %d\n", getpid());
  pnd_io_t ctx;
  pnd_io_init(&ctx);

  pnd_tcp_t *server = malloc(sizeof(pnd_tcp_t));
  pnd_tcp_init(&ctx, server);
  pnd_tcp_listen(server, 8000, handle_connection);

  pnd_io_run(&ctx);
  
  return 0;
}

void on_server_response(pnd_tcp_t *stream) {
  char buf[1024] = {};
  ssize_t bytes = read(stream->fd, buf, 1023);
  if (bytes < 0) {
    perror("read");
    return;
  }

  if (bytes == 0) {
    //pnd_tcp_close(stream);
    printf("Received 0 bytes for: %d\n", stream->fd);
    pnd_tcp_destroy(stream);
    return;
  }

  printf("Response from server: %s\n", buf);

  pnd_tcp_close(stream);
}

void handle_connect(pnd_tcp_t *client, pnd_fd_t fd) {
  printf("Connected to server. Client fd: %d\n", fd);
  
  client->on_data = on_server_response;
  client->on_close = handle_close;

  pnd_write_t *write_op = malloc(sizeof(pnd_write_t));
  pnd_tcp_write_init(write_op, strdup("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"), 35, handle_write);
  pnd_tcp_write(client, write_op);
}

int main() {
  printf("Starting client, pid is: %d\n", getpid());
  pnd_io_t ctx;
  pnd_io_init(&ctx);

  pnd_tcp_t *client = malloc(sizeof(pnd_tcp_t));
  pnd_tcp_init(&ctx, client);
  pnd_tcp_connect(client, "127.0.0.1", 3000, handle_connect);

  pnd_io_run(&ctx);
  
  return 0;
}
