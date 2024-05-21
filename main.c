#include <stdbool.h>

#include "socket.c"
#include "events.c"
#include "logger.c"

#define DEFAULT_PORT 8080
#define DEFAULT_WORKERS 4
#define MAX_EVENTS 128

#define BUFFER_SIZE 1024

void handle_connection(int fd) {
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(fd, buffer, BUFFER_SIZE, 0);
    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (bytes_read == 0) {
        close(fd);
        return;
    }

    buffer[bytes_read] = '\0';
    //printf("Received: %s\n", buffer);

    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 22\r\nContent-Type: text/html\r\n\r\n<h1>Hello world!</h1>\n";
    send(fd, response, strlen(response), 0);
    close(fd);
}

void worker(int listener_fd) {
    int epoll_fd = init_epoll();
    add_to_epoll(epoll_fd, listener_fd);
    struct epoll_event events[MAX_EVENTS];

    while(true) {
        int events_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (events_count == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < events_count; i++) {
            if (events[i].data.fd == listener_fd) {
                int client_fd = accept_peer(listener_fd);
                if (client_fd == -1) {
                    continue;
                }
                add_to_epoll(epoll_fd, client_fd);
            } else {
                handle_connection(events[i].data.fd);
            }
        }
    }
}

int getenv_int(const char *name, int default_value) {
    char *value = getenv(name);
    if (value == NULL) {
        return default_value;
    }
    return atoi(value);
}

int main() {
    int port = getenv_int("PORT", DEFAULT_PORT);
    int workers = getenv_int("WORKERS", DEFAULT_WORKERS);
    int listener_fd = init_listener(port);

    for (int i = 0; i < workers; i++) {
        int pid = fork();
        if (pid == 0) {
            info("Worker started");
            worker(listener_fd);
            exit(EXIT_SUCCESS);
        }
    }

    info("HTTP server started");
    printf("Listening on port %d\n", port);

    while(true) {
        // waiting for signals etc...
    }
}
