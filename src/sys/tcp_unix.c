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
void pnd__tcp_listener_io(pd_event_t *event, unsigned events) {
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
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int opt = 1;

    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        pd__closesocket(lfd);
        return -1;
    }

    if (bind(lfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        pd__closesocket(lfd);
        return -1;
    }

    if (pd_set_nonblocking(lfd) < 0) {
        pd__closesocket(lfd);
        return -1;
    }

    if (listen(lfd, SOMAXCONN) < 0) {
        pd__closesocket(lfd);
        return -1;
    }

    server->fd = lfd;
    server->event.handler = pnd__tcp_listener_io;
    server->status = PD_TCP_ACTIVE;
    server->on_connection = on_connection;

    pd_event_read_start(server->ctx, &server->event, lfd);

    return 0;
}


// handler for read event from epoll/kqueue
void pd__tcp_read(pd_tcp_t *stream) {
    if (!stream->on_data) {
        return;
    }

    size_t read_size = 6 * 1024;
    ssize_t nread;

    // TODO: make callback for own allocations/providing own buffer
    char *buf = malloc(read_size);
    if (buf == NULL) {
        stream->on_data(stream, buf, -1); // in this scenario - we have no memory for new buffer
    }

    do {
        nread = read(stream->fd, buf, read_size);
    } while (nread == -1 && errno == EINTR);

    if (nread == 0) {
        // TODO: close connection
    }

    if (nread < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // it's not error, but we have to report it in callback, because user should call free() on buf
            stream->on_data(stream, buf, 0);
            return;
        } else {
            stream->on_data(stream, buf, -1);
        }
    } else {
        stream->on_data(stream, buf, nread);
    }
}


/* Actually writes enqueued writes */
void pd__tcp_write(pd_tcp_t *stream)
{
    // sometimes we get stale event when stream was closed in the same iteration
    if (stream->status == PD_TCP_CLOSED)
        return;

    while (!queue_empty(&stream->writes)) {
        struct queue_node *next = queue_peek(&stream->writes);
        pd_write_t *write_op = container_of(next, pd_write_t, qnode);

        size_t to_write = write_op->data.len - write_op->written;
        const char *buf = write_op->data.buf + write_op->written;
        ssize_t written;

        do {
            written = write(stream->fd, buf, to_write);
        } while (written < 0 && errno == EINTR);

        if (written < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            } else {
                perror("write");
                queue_pop(&stream->writes);
                write_op->cb(write_op, -1);
                return;
            }
        }

        write_op->written += written;

        if (write_op->written == write_op->data.len) {
            queue_pop(&stream->writes);
            if (write_op->cb)
                write_op->cb(write_op, 0);
        }
    }

    if (queue_empty(&stream->writes)) {
        pd_event_write_stop(stream->ctx, &stream->event, stream->fd);

        if (stream->status == PD_TCP_SHUTDOWN)
            shutdown(stream->fd, SHUT_WR);
    }
}


/* handler for I/O events from epoll/kqueue */
void pd__tcp_client_io(pd_event_t *event, unsigned events) {
    pd_tcp_t *stream = container_of(event, pd_tcp_t, event);

    if (events & PD_POLLIN) {
        pd__tcp_read(stream);
    }

    if (events & PD_CLOSE) {
        // TODO: end tcp stream
    }

    if (events & PD_POLLOUT) {
        pd__tcp_write(stream);
    }
}


void pd_tcp_accept(pd_tcp_t *peer, pd_socket_t fd) {
    peer->fd = fd;
    peer->status = PD_TCP_ACTIVE;
    peer->event.handler = pd__tcp_client_io;
    pd_event_read_start(peer->ctx, &peer->event, peer->fd);
}


void pd__tcp_connect_io(pd_event_t *event, unsigned events) {
    pd_tcp_t *stream = container_of(event, pd_tcp_t, event);

    if (events & PD_CLOSE) {
        // TODO: close connection
    }

    if (events & PD_POLLOUT) {
        stream->status = PD_TCP_ACTIVE;
        event->handler = pd__tcp_client_io;

        pd_event_read_only(stream->ctx, event, stream->fd);

        if (stream->on_connect != NULL)
            stream->on_connect(stream);
    }
}


int pd_tcp_connect(pd_tcp_t *stream, const char *host, int port, void (*on_connect)(pd_tcp_t*)) {
    pd_socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    if (pd_set_nonblocking(fd) < 0) {
        pd__closesocket(fd);
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host);

    stream->fd = fd;
    stream->on_connect = on_connect;
    stream->event.handler = pd__tcp_connect_io;

    if (connect(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        if (errno != EINPROGRESS) {
            pd__closesocket(fd);
            return -1;
        }
    }

    pd_event_write_start(stream->ctx, &stream->event, fd);

    return 0;
}


void pd_tcp_pause(pd_tcp_t *stream) {
    pd_event_read_stop(stream->ctx, &stream->event, stream->fd);
}


void pd_tcp_resume(pd_tcp_t *stream) {
    pd_event_read_start(stream->ctx, &stream->event, stream->fd);
}


void pd_write_init(pd_write_t *write_op, char *buf, size_t size, pd_write_cb cb) {
    write_op->data.buf = buf;
    write_op->data.len = size;
    write_op->written = 0;
    write_op->cb = cb;
    //write_op->data = NULL;
    queue_init_node(&write_op->qnode);
}


/* Enqueue write operation and do it asynchronously.
 * Usually requires copying data to the new structure
 */
void pd_tcp_write_async(pd_tcp_t *stream, pd_write_t *write_op) {
    if (stream->status != PD_TCP_ACTIVE) {
        write_op->cb(write_op, -1);
        return;
    }

    queue_init_node(&write_op->qnode);
    queue_push(&stream->writes, &write_op->qnode);
}


void pd_tcp_write(pd_tcp_t *stream, pd_write_t *write_op) {
    if (stream->status != PD_TCP_ACTIVE) {
        write_op->cb(write_op, -1);
        return;
    }

    if (queue_empty(&stream->writes)) {
        ssize_t written;
        do {
            written = write(stream->fd, write_op->data.buf, write_op->data.len);
        } while (written < 0 && errno == EINTR);

        if (written < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                pd_tcp_write_async(stream, write_op);
                return;
            }

            write_op->cb(write_op, -1);
            return;
        }

        // partial writes:
        if (written < write_op->data.len) {
            write_op->written += written;
            pd_tcp_write_async(stream, write_op);
        } else {
            write_op->cb(write_op, 0);
        }
    } else {
        pd_tcp_write_async(stream, write_op);
    }
}
