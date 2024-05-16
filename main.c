#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "logger.c"
#include <stdbool.h>

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

  int listen_status = listen(socketd, 3);
  if (listen_status < 0) {
    perror("listen");
    close(socketd);
    die("Unable to listen on port");
  }

  while (true) {
    int request_socket = accept(socketd, NULL, NULL);
    if (request_socket < 0) {
      perror("accept");
      close(socketd);
      die("Unable to accept request");
    }

    char buffer[BUFFER_SIZE] = {0};
    int valread = read(request_socket, buffer, BUFFER_SIZE);

    info("Received HTTP request");

    const char *http_response = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: text/html\r\n"
                              "Content-Length: 20\r\n"
                              "Connection: Closed\r\n"
                              "\r\n"
                              "<h1>Hello World</h1>";

    send(request_socket, http_response, strlen(http_response), 0);
    close(request_socket);
    printf("Received: %s\n", buffer);
  }


  info("Closing socket...");
  close(socketd);
  return 0;
}
