#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>

#include "logger.c"
#include "request.c"
#include "request_parser.c"

#define PORT 8080
#define BUFFER_SIZE (5 * 1024)

int main() {
  info("Starting HTTP server....");

  int socketd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketd <= 0) {
    die("Socket creation failed");
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  int opt = 1;
  if (setsockopt(socketd, SOL_SOCKET,
                   SO_REUSEADDR, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

  int bind_status = bind(socketd, (struct sockaddr *)&address, sizeof(address));
  if (bind_status < 0) {
    perror("bind");
    close(socketd);
    die("Unable to bind port");
  }

  int listen_status = listen(socketd, SOMAXCONN);
  if (listen_status < 0) {
    perror("listen");
    close(socketd);
    die("Unable to listen on port");
  }

  // TODO: make event loop work with concurrent clients
  while (true) {
    int request_socket = accept(socketd, NULL, NULL);
    if (request_socket < 0) {
      perror("accept");
      continue;
    }

    char buffer[BUFFER_SIZE] = {0};
    size_t read_size = read(request_socket, buffer, BUFFER_SIZE);
    RequestParser parser = init_parser(buffer);
    print_request(&parser);
    printf("Read size: %ld\n", read_size);

    const char *http_response = "HTTP/1.0 200 OK\r\n"
                              "Content-Type: text/html\r\n"
                              "Content-Length: 20\r\n"
                              "Connection: close\r\n"
                              "\r\n"
                              "<h1>Hello World</h1>";

    send(request_socket, http_response, strlen(http_response), 0);
    shutdown(request_socket, SHUT_RDWR);
    close(request_socket);
  }

  info("Closing socket...");
  close(socketd);
  return 0;
}
