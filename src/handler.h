#pragma once
#include "htable.h"
#include "http_request.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void fmt_response(char *res, size_t buffer_size) {
    char *body = strdup(res);
    if (body == NULL) return;

    int n = snprintf(NULL, 0, "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s", strlen(body), body);
    if (n > 0 && n < buffer_size) {
        snprintf(res, buffer_size, "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s", strlen(body), body);
    }
    free(body);
}

static char *handle_request(http_request_t *req) {
    const size_t response_size = 2048;
    char *response = malloc(response_size);
    if (response == NULL) return NULL;

    char *hostname = htable_get(&req->headers, "Host");

    if (req->method == GET) {
        snprintf(response, response_size, "GET request to hostname: %s and path: %s", hostname, req->path);
    } else if (req->method == POST) {
        snprintf(response, response_size, "POST request to %s", req->path);
    } else {
        snprintf(response, response_size, "Unknown request method");
    }

    fmt_response(response, response_size);
    return response;
}
