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
#include <stdio.h>

// how many times issue AcceptEx?
#define SIMULTANEOUS_ACCEPTS 10

/* private and Windows-specific structure to hold async accept requests */
struct pd__accept_op_s {
    SOCKET socket;  // accepted socket
    pd_event_t event;
    pd_tcp_server_t *server;
    char buf[(sizeof(struct sockaddr_storage) + 16) * 2];
    struct queue_node qnode; // we hold pool of accept requests as linked-list
};
typedef struct pd__accept_op_s pd__accept_op_t;

void pd__tcp_post_acceptex(pd_tcp_server_t *server, pd__accept_op_t *op);
void pd__tcp_post_recv(pd_tcp_t *stream);


void pd_tcp_server_init(pd_io_t *ctx, pd_tcp_server_t *server) {
    server->ctx = ctx;
    server->fd = INVALID_SOCKET;
    server->on_connection = NULL;
    server->acceptex = NULL;
}


/* Handling callback from IOCP */
void pd__tcp_accept_io(pd_event_t *event) {
    pd__accept_op_t *op = event->data;
    pd_tcp_server_t *server = op->server;

    if (server->on_connection)
        server->on_connection(server, op->socket, 0);

    pd__tcp_post_acceptex(server, op);
}


void pd__tcp_post_acceptex(pd_tcp_server_t *server, pd__accept_op_t *op) {
    op->socket = socket(AF_INET, SOCK_STREAM, 0);
    op->server = server;
    queue_init_node(&op->qnode);
    pd_event_init(&op->event);
    op->event.handler = pd__tcp_accept_io;
    op->event.data = op;
    DWORD ret;

    BOOL success = server->acceptex(
            server->fd, op->socket, op->buf, 0,
            sizeof(struct sockaddr_storage) + 16, sizeof(struct sockaddr_storage) + 16,
            &ret, &op->event.overlapped);

    if (!success && WSAGetLastError() != WSA_IO_PENDING) {
        closesocket(op->socket);
    }
}


int pd_tcp_listen(pd_tcp_server_t *server,
                   int port,
                   pd_on_connection_cb on_connection) {

    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    server->on_connection = on_connection;

    if (server->fd == INVALID_SOCKET) {
        printf("Unable to initialize socket\n");
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    const char opt = 1;
    if (setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        closesocket(server->fd);
        return -1;
    }

    if (bind(server->fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        closesocket(server->fd);
        return -1;
    }

    if (listen(server->fd, 1024) < 0) {
        closesocket(server->fd);
        return -1;
    }

    /* we need to load dynamically MS extension to get AcceptEx function */
    if (!server->acceptex) {
        GUID guid = WSAID_ACCEPTEX;
        DWORD ret;
        BOOL res = WSAIoctl(
                server->fd,
                SIO_GET_EXTENSION_FUNCTION_POINTER,
                &guid, sizeof(guid),
                &server->acceptex, sizeof(server->acceptex),
                &ret, NULL, NULL);

        if (res == SOCKET_ERROR) {
            closesocket(server->fd);
            return -1;
        }
    }

    if (CreateIoCompletionPort((HANDLE)server->fd, server->ctx->poll_fd, 0, 0) == NULL) {
        closesocket(server->fd);
        return -1;
    }

    for (int i = 0; i < SIMULTANEOUS_ACCEPTS; ++i) {
        pd__accept_op_t *op = malloc(sizeof(pd__accept_op_t));
        pd__tcp_post_acceptex(server, op);
    }

    return 0;
}


void pd_tcp_init(pd_io_t *ctx, pd_tcp_t *stream) {
    stream->fd = INVALID_SOCKET;
    stream->status = PD_TCP_NONE;
    stream->ctx = ctx;
    stream->on_close = NULL;
    stream->on_data = NULL;
    stream->on_connect = NULL;
    stream->writes_size = 0;
    stream->flags = 0;
}


void pd_tcp_accept(pd_tcp_t *stream, pd_socket_t socket) {
    stream->status = PD_TCP_ACTIVE;
    stream->fd = socket;
    CreateIoCompletionPort((HANDLE)stream->fd,
                           stream->ctx->poll_fd, (ULONG_PTR)stream, 0);
    pd__tcp_post_recv(stream);
}


void pd__tcp_try_close(pd_tcp_t *stream) {
    if (stream->writes_size > 0) {
        return;
    }

    if (stream->flags & PD_PENDING_READ) {
        return;
    }

    closesocket(stream->fd);
    CloseHandle((HANDLE)stream->fd);

    queue_init_node(&stream->close_qnode);
    queue_push(&stream->ctx->pending_closes, &stream->close_qnode);
}


// Completion callback from IOCP for write operation
void pd__tcp_write_io(pd_event_t *event) {
    pd_write_t *write_op = event->data;
    pd_tcp_t *stream = write_op->handle;

    if (write_op->cb)
        // TODO: detect errors
        write_op->cb(write_op, 0);

    stream->writes_size--;

    if (stream->writes_size == 0) {
        // stream is not writable because have no data to write anymore
        stream->flags &= ~PD_WRITING;
    }

    if (stream->status == PD_TCP_CLOSED) {
        pd__tcp_try_close(stream);
    }
}


void pd_write_init(pd_write_t *write_op,
                   char *buf, size_t size,
                   pd_write_cb cb) {
    write_op->cb = cb;
    write_op->data.buf = buf;
    write_op->data.len = size;
    pd_event_init(&write_op->event);
    write_op->event.data = write_op;
    write_op->event.handler = pd__tcp_write_io;
}


void pd_tcp_write(pd_tcp_t *stream, pd_write_t *write_op) {
    if (stream->status != PD_TCP_ACTIVE) {
        write_op->cb(write_op, -1);
        return;
    }

    DWORD bytes;
    int status = WSASend(stream->fd,
            &write_op->data, 1, &bytes, 0,
            &write_op->event.overlapped, NULL);

    write_op->handle = stream;

    if (status == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        //  TODO: map system error codes to library specific codes
        write_op->cb(write_op, -1);
        return;
    } else {
        stream->writes_size++;
        stream->flags |= PD_WRITING;
    }
}


void pd_tcp_close(pd_tcp_t *stream) {
    if (stream->status == PD_TCP_CLOSED)
        return;

    stream->status = PD_TCP_CLOSED;

    if (stream->writes_size > 0) {
        CancelIo((HANDLE)stream->fd);
    }

    if (stream->flags & PD_PENDING_READ) {
        CancelIoEx((HANDLE)stream->fd, &stream->revent.overlapped);
    }

    stream->flags &= ~(PD_READING | PD_WRITING);

    pd__tcp_try_close(stream);
}


void pd__tcp_read_io(pd_event_t *event) {
    pd_tcp_t *stream = event->data;
    stream->flags &= ~PD_PENDING_READ;

    if (event->bytes > 0) {
        stream->on_data(stream, stream->read_buf.buf, event->bytes);
    } else {
        pd_tcp_close(stream);
        return;
    }

    if (stream->status == PD_TCP_CLOSED) {
        pd__tcp_try_close(stream);
        return;
    }

    if (stream->flags & PD_READING) {
        pd__tcp_post_recv(stream);
    }
}


void pd__tcp_post_recv(pd_tcp_t *stream) {
    assert(stream->fd != INVALID_SOCKET);

    pd_event_init(&stream->revent);
    stream->revent.data = stream;
    stream->revent.handler = pd__tcp_read_io;

    size_t alloc_size = 8 * 1024;
    stream->read_buf.buf = malloc(alloc_size);
    stream->read_buf.len = alloc_size;

    DWORD bytes;
    DWORD flags = 0;
    int status = WSARecv(stream->fd,
            &stream->read_buf, 1,
            &bytes, &flags,
            &stream->revent.overlapped, NULL);

    if (status == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        // TODO: handle this error
        return;
    } else {
        stream->flags |= PD_READING;
        stream->flags |= PD_PENDING_READ;
    }
}


void pd_tcp_pause(pd_tcp_t *stream) {
    // it will prevent requesting more data
    stream->flags &= ~PD_READING;

    if (stream->flags & PD_PENDING_READ) {
        CancelIoEx((HANDLE)stream->fd, &stream->revent.overlapped);
    }
}


void pd_tcp_resume(pd_tcp_t *stream) {
    stream->flags |= PD_READING;

    if ((stream->flags & PD_PENDING_READ) == 0) {
        pd__tcp_post_recv(stream);
    }
}


// callback for IOCP
void pd__tcp_connect_io(pd_event_t *event) {
    pd_tcp_t *stream = event->data;
    free(event); // after successful connection, we do not need event object

    pd__tcp_post_recv(stream);
    stream->status = PD_TCP_ACTIVE;
    if (stream->on_connect)
        stream->on_connect(stream);
}


static LPFN_CONNECTEX pd__connectex = NULL;

int pd_tcp_connect(pd_tcp_t *stream, const char *host, int port, void (*on_connect)(pd_tcp_t*)) {
    pd_socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == INVALID_SOCKET) {
        return -1;
    }

    stream->fd = fd;
    stream->on_connect = on_connect;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &address.sin_addr) <= 0) {
        closesocket(fd);
        return -1;
    }

    struct sockaddr_in local_addr = { 0 };
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = 0;

    if (bind(fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) {
        closesocket(fd);
        return -1;
    }
    // load ConnectEx function only once
    if (!pd__connectex) {
        GUID guid = WSAID_CONNECTEX;
        DWORD bytes = 0;

        int loaded = WSAIoctl(fd, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid),
                     &pd__connectex, sizeof(pd__connectex), &bytes, NULL, NULL);

        if (loaded == SOCKET_ERROR) {
            closesocket(fd);
            return -1;
        }
    }

    pd_event_t *connect_ev = malloc(sizeof(pd_event_t));
    // not enough memory:
    if (!connect_ev) {
        closesocket(fd);
        return -1;
    }

    if (CreateIoCompletionPort((HANDLE)fd, stream->ctx->poll_fd, 0, 0) == NULL) {
        free(connect_ev);
        closesocket(fd);
        return -1;
    }

    pd_event_init(connect_ev);
    connect_ev->data = stream;
    connect_ev->handler = pd__tcp_connect_io;
    int status = pd__connectex(fd, (struct sockaddr*)&address, sizeof(struct sockaddr_in),
            NULL, 0, NULL, &connect_ev->overlapped);

    if (status == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        free(connect_ev);
        closesocket(fd);
        CloseHandle((HANDLE)fd);
        return -1;
    }

    return 0;
}
