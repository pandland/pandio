/* Copyright (c) Michał Dziuba
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "tcp.h"
#include "common.h"
#include <stdio.h>


void pd_tcp_init(pd_io_t *ctx, pd_tcp_t *stream) {
    stream->fd = -1;
    stream->status = PD_TCP_NONE;
    stream->ctx = ctx;
    stream->on_close = NULL;
    stream->on_data = NULL;
    stream->flags = 0;
    stream->writes_size = 0;

    queue_init(&stream->writes);
    pd_event_init(&stream->event);
}


// over time, we will probably make shared function across unixes called pd__close_fd
int pd__closesocket(pd_socket_t fd){
    int status;
    do {
        status = close(fd);
    } while (status == -1 && errno == EINTR);

    return status;
}


void pd_tcp_server_init(pd_io_t *ctx, pd_tcp_server_t *server) {
    server->ctx = ctx;
    server->fd = -1;
    server->on_connection = NULL;
    server->data = NULL;
    server->status = PD_TCP_NONE;
    pd_event_init(&server->event);
}


/* handler for I/O events from epoll/kqueue */
void pnd_tcp_listener_io(pd_event_t *event, unsigned events) {
    assert(events & PD_POLLOUT);

    pd_tcp_server_t *listener = container_of(event, pd_tcp_server_t, event);

    if (listener->status != PD_TCP_ACTIVE) {
        return;
    }

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    pd_socket_t peer_fd = accept(listener->fd, (struct sockaddr *)&address, &addrlen);
    if (peer_fd < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // ignore error and wait for another pd_tcp_listener_io call...
            return;
        } else {
            perror("accept");
            return;
        }
    }

    if (pd_set_nonblocking(peer_fd) < 0) {
        pd__closesocket(peer_fd);
        return;
    }

    if (listener->on_connection != NULL) {
        listener->on_connection(listener, peer_fd, 0);
    } else {
        // no handler set, so close socket to avoid fd leak.
        pd__closesocket(peer_fd);
    }
}


int pd_tcp_listen(pd_tcp_server_t *server,
                  int port,
                  pd_on_connection_cb on_connection) {

    pd_socket_t lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd <= 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int opt = 1;

    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        pd__closesocket(lfd);
        return -1;
    }

    if (bind(lfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        pd__closesocket(lfd);
        return -1;
    }

    if (pd_set_nonblocking(lfd) < 0) {
        pd__closesocket(lfd);
        return -1;
    }

    if (listen(lfd, SOMAXCONN) < 0) {
        perror("listen");
        pd__closesocket(lfd);
        return -1;
    }

    server->fd = lfd;
    server->event.handler = pnd_tcp_listener_io;
    server->status = PD_TCP_ACTIVE;
    server->on_connection = on_connection;

    pd_event_read_start(server->ctx, &server->event, lfd);

    return 0;
}


/* handler for I/O events from epoll/kqueue */
void pnd_tcp_client_io(pd_event_t *event, unsigned events)
{
    pd_tcp_t *stream = container_of(event, pd_tcp_t, event);

    if (events & PD_CLOSE) {
        printf("Detected PND_CLOSE\n");
        // TODO: pd_tcp_close(stream);
        return;
    }

    if (events & PD_POLLIN) {
        // TODO: actually read data and pass chunk to the on_data callback, instead
        stream->on_data(stream, NULL, 0);
    }

    if (events & PD_POLLOUT) {
        // TODO: pd_tcp_write_io(stream);
    }
}


void pd_tcp_accept(pd_tcp_t *peer, pd_socket_t fd) {
    peer->fd = fd;
    peer->status = PD_TCP_ACTIVE;
    peer->event.handler = pnd_tcp_client_io;
    pd_event_read_start(peer->ctx, &peer->event, peer->fd);
}
