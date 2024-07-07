#pragma once
#include "htable.h"

enum HTTP_METHOD {
  GET,
  POST,
  PUT,
  PATCH,
  DELETE,
  OPTIONS
};

static char *map_method(enum HTTP_METHOD method) {
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

typedef enum HTTP_METHOD http_method_t;

struct http_request_s {
  char *path;
  http_method_t method;
  htable_t headers;
  char *body;
};

typedef struct http_request_s http_request_t;

static http_request_t *http_request_alloc() {
  http_request_t *req = malloc(sizeof(http_request_t));
  req->headers = htable_create();
  req->path = NULL;
  req->body = NULL;

  return req;
}

static void http_request_destroy(http_request_t *req) {
  free(req->path);
}

static void http_request_free(http_request_t *req) {
  http_request_destroy(req);
  free(req);
}
