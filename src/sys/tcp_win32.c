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
#define PENDING_ACCEPTS 10

/* private and Windows-specific structure to hold async accept requests */
struct pd__accept_op_s {
    SOCKET socket;  // accepted socket
    pd_event_t event;
    pd_tcp_server_t *server;
    char buf[(sizeof(struct sockaddr_storage) + 16) * 2];
    struct queue_node qnode; // we hold pool of accept requests as a queue (it is just a linked list)
};
typedef struct pd__accept_op_s pd__accept_op_t;


void pd_tcp_server_init(pd_io_t *ctx, pd_tcp_server_t *server) {
    server->ctx = ctx;
    server->fd = INVALID_SOCKET;
    server->on_connection = NULL;
    server->accept = NULL;
}


/* Handling callback from IOCP */
void pd__tcp_accept_io(pd_event_t *event) {
    pd__accept_op_t *op = event->data;
    pd_tcp_server_t *server = op->server;

    if (server->on_connection)
        server->on_connection(server);
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
        // TODO: close socket
        return -1;
    }

    if (bind(server->fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        // TODO: close socket
        return -1;
    }

    if (listen(server->fd, 1024) < 0) {
        // TODO: close socket
        return -1;
    }

    /* we need to load dynamically MS extension to get AcceptEx function */
    if (!server->accept) {
        GUID guid = WSAID_ACCEPTEX;
        DWORD ret;
        BOOL res = WSAIoctl(
                server->fd,
                SIO_GET_EXTENSION_FUNCTION_POINTER,
                &guid, sizeof(guid),
                &server->accept, sizeof(server->accept),
                &ret, NULL, NULL);

        if (res == SOCKET_ERROR) {
            printf("Unable to load AcceptEx function\n");
            // TODO: close socket
            return -1;
        }
    }

    if (CreateIoCompletionPort((HANDLE)server->fd, server->ctx->poll_fd, 0, 0) == NULL) {
        printf("CreateIoCompletionPort failed with error: %lu\n", GetLastError());
        // TODO: close socket
        return -1;
    }

    for (int i = 0; i < PENDING_ACCEPTS; ++i) {
        pd__accept_op_t *op = malloc(sizeof(pd__accept_op_t));
        op->socket = socket(AF_INET, SOCK_STREAM, 0);
        op->server = server;
        queue_init_node(&op->qnode);
        pd_event_init(&op->event);
        op->event.handler = pd__tcp_accept_io;
        op->event.data = op;

        DWORD ret;

        server->accept(
                server->fd, op->socket, op->buf, 0,
                sizeof(struct sockaddr_storage) + 16, sizeof(struct sockaddr_storage) + 16,
                &ret, &op->event.overlapped);
    }

    return 0;
}
