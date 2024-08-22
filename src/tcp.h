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

#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>
#include <ws2def.h>
#include <mswsock.h>

typedef SOCKET pd_socket_t;

#else

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
typedef int pd_socket_t;

#endif

#include "core.h"
#include "queue.h"

/* struct that represents TCP connection and stream */
struct pd_tcp_s {
    pd_io_t *ctx;
    pd_socket_t fd;
    void (*on_data)(struct pd_tcp_s *);
    void (*on_close)(struct pd_tcp_s *);
#ifndef _WIN32
    struct queue writes;
    size_t writes_size;
#else
    size_t pending_ops;
#endif
};

typedef struct pd_tcp_s pd_tcp_t;

struct pd_tcp_server_s;
typedef void (*pd_on_connection_cb)(struct pd_tcp_server_s*, pd_socket_t, int);

/* struct that represents TCP server/listener */
struct pd_tcp_server_s {
    pd_io_t *ctx;
    pd_socket_t fd;
    pd_on_connection_cb on_connection;
#ifdef _WIN32
    LPFN_ACCEPTEX acceptex;
    struct queue accept_ops;
#endif
};

typedef struct pd_tcp_server_s pd_tcp_server_t;

struct pd_write_s;

typedef void (*pd_write_cb)(struct pd_write_s*, int status);

struct pd_write_s {
#ifdef _WIN32
    WSABUF data;
    pd_event_t event;
    pd_tcp_t *handle;
#else
    char *buf;
    size_t size;
#endif
    pd_write_cb cb;
};

typedef struct pd_write_s pd_write_t;

int pd_tcp_listen(pd_tcp_server_t*, int, pd_on_connection_cb);

void pd_tcp_server_init(pd_io_t*, pd_tcp_server_t*);

void pd_tcp_init(pd_io_t*, pd_tcp_t*);

/* Creates pd_tcp_t from OS socket handle */
void pd_tcp_accept(pd_tcp_t*, pd_socket_t);

void pd_write_init(pd_write_t*, char*, size_t, pd_write_cb);

void pd_tcp_write(pd_tcp_t*, pd_write_t*);
