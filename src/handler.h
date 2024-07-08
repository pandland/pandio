#pragma once
#include "htable.h"
#include "http_request.h"

static char *handle_request(http_request_t *req) {
  char *response = malloc(128);
  char *hostname = htable_get(&req->headers, "Host");

  if (req->method == GET) {
    sprintf(response, "GET request to hostname: %s and path: %s", hostname, req->path);
  } else if (req->method == POST) {
    sprintf(response, "POST request to %s", req->path);
  } else {
    sprintf(response, "Unknown request method");
  }

  return response;
}
