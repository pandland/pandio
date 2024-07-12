#pragma once
#include "htable.h"
#include "net.h"

enum HTTP_METHOD {
  GET,
  POST,
  PUT,
  PATCH,
  DELETE,
  OPTIONS
};

typedef enum HTTP_METHOD http_method_t;

static char *map_method(http_method_t method) {
  switch (method)
  {
  case 0:
    return "GET";
  case 1:
    return "POST";
  case 2:
    return "PUT";
  case 3:
    return "PATCH";
  case 4:
    return "DELETE";
  case 5:
    return "OPTIONS";
  default:
    return "UNKNOWN";
  }
}

struct http_request_s {
  char *path;
  http_method_t method;
  htable_t headers;
  char *body;
  lxe_connection_t *connection;
};

typedef struct http_request_s http_request_t;

static http_request_t *http_request_alloc() {
  http_request_t *req = malloc(sizeof(http_request_t));
  req->headers = htable_create();
  req->path = NULL;
  req->body = NULL;
  req->connection = NULL;

  return req;
}

static void http_request_destroy(http_request_t *req) {
  free(req->path);
  free(req->body);
  htable_destroy(&req->headers);
}

static void http_request_free(http_request_t *req) {
  http_request_destroy(req);
  free(req);
}
