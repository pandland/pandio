#pragma once

#include "net.h"
#include "timer.h"
#include "http_parser.h"

enum HTTP_METHOD {
  GET,
  POST,
  PUT,
  PATCH,
  DELETE,
  OPTIONS,
  HEAD,
  TRACE,
  CONNECT
};

typedef enum HTTP_METHOD http_method_t;

static char *http_map_method(http_method_t method) {
  switch (method)
  {
  case GET:
    return "GET";
  case POST:
    return "POST";
  case PUT:
    return "PUT";
  case PATCH:
    return "PATCH";
  case DELETE:
    return "DELETE";
  case OPTIONS:
    return "OPTIONS";
  case HEAD:
    return "HEAD";
  case TRACE:
    return "TRACE";
  case CONNECT:
    return "CONNECT";
  default:
    return "UNKNOWN";
  }
}

#define MAX_HEADERS 64

typedef struct http_raw_header {
  slice_t key;
  slice_t value;
} http_raw_header_t;

struct http_request_s {
  char *path;
  http_method_t method;
  unsigned persistent: 1;
  unsigned upgrade: 1;
  char *body;
  size_t body_size;
  lx_http_parser_t parser;
  lx_connection_t *connection;
  lx_timer_t timeout;
  http_raw_header_t raw_headers[MAX_HEADERS];
};

typedef struct http_request_s http_request_t;

static http_request_t *http_request_alloc() {
  http_request_t *req = malloc(sizeof(http_request_t));
  req->path = NULL;
  req->body = NULL;
  req->connection = NULL;
  lx_http_parser_init(&req->parser);
  req->parser.req = req;
  req->persistent = 1;
  req->upgrade = 0; // it is only indicator that client may want to upgrade protocol

  return req;
}

static void http_request_destroy(http_request_t *req) {
  free(req->path);
  free(req->body);
}

static void http_request_free(http_request_t *req) {
  http_request_destroy(req);
  free(req);
}
