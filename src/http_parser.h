#pragma once
#include "slice.h"

typedef enum lx_parser_status {
  LX_COMPLETE,
  LX_PARTIAL,
  // error codes:
  LX_UNEXPECTED_ERROR, // if something will go really bad within implementation
  LX_INVALID_METHOD,
  LX_INVALID_URI,
  LX_INVALID_CHAR,
  LX_INVALID_HEADER_KEY_CHAR,
  LX_INVALID_HEADER_VALUE_CHAR,
  LX_TOO_MANY_HEADERS,
  LX_INVALID_CONTENT_LENGTH,
  LX_CONTENT_LENGTH_DUPLICATE
} lx_parser_status_t;

typedef struct lx_buf {
  unsigned char *buf;
  size_t size;
} lx_buf_t;

typedef enum lx_parser_state {
  // line section
  line_start,
  method,
  method_end,
  uri_start,
  uri,
  uri_end,
  version_start,
  version_h,
  version_ht,
  version_htt,
  version_http,
  version_http_slash,
  version_http_major,
  version_http_dot,
  version_http_minor,
  version_end,
  line_end,
  // headers section
  header_key_start,
  header_key,
  header_key_end,
  header_space,
  header_value_start,
  header_value,
  header_value_end,
  header_end,
  maybe_end,
  end
} lx_parser_state_t;

typedef struct http_request_s http_request_t;

typedef struct lx_http_parser {
  size_t nread;
  lx_parser_state_t state;
  http_request_t *req;
  slice_t uri;
  slice_t method;
  slice_t header_key;
  slice_t header_value;
  size_t content_length;
  int content_length_received: 1;
  unsigned nheaders;
} lx_http_parser_t;

const char* lx_http_map_code(lx_parser_status_t);
void lx_http_parser_init(lx_http_parser_t *parser);
int lx_http_parser_exec(lx_http_parser_t *parser, lx_buf_t *data);
