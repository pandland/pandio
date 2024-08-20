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

#include <stdio.h>
#include "core.h"
#include "timers.h"

uint64_t pd_now() {
    FILETIME ft;
    ULARGE_INTEGER li;

    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    return li.QuadPart / 10000ULL;
}

void pd_io_init(pd_io_t *ctx) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    ctx->poll_fd = CreateIoCompletionPort(
            INVALID_HANDLE_VALUE,NULL,
            0, 0);

    ctx->now = 0;
    pd_timers_heap_init(ctx);
}

void pd_event_init(pd_event_t *event) {
    event->handler = NULL;
    event->data = NULL;
    event->flags = 0;
    ZeroMemory(&event->overlapped, sizeof(event->overlapped));
}

void pd_io_run(pd_io_t *ctx) {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    pd_event_t *ev = NULL;

    while (TRUE) {
        BOOL result = GetQueuedCompletionStatus(
                ctx->poll_fd,
                &bytesTransferred,
                &completionKey,
                (LPOVERLAPPED *)&ev,
                INFINITE);

        printf("We got something\n");
        if (!result || !ev) {
            // TODO: handle errors in better way
            printf("GetQueueCompletionStatus failed\n");
            break;
        }

        printf("Received event\n");
        if (ev->handler)
            ev->handler(ev);
    }
}
