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

typedef enum HTTP_METHOD http_method_t;

struct http_request_s {
  char *path;
  http_method_t method;
  htable_t headers;
  char *body;
};

typedef struct http_request_s http_request_t;

static http_request_t *http_request_alloc() {
  return malloc(sizeof(http_request_t));
}

static void http_request_destroy(http_request_t *req) {
  free(req->path);
}

static void http_request_free(http_request_t *req) {
  http_request_destroy(req);
  free(req);
}
