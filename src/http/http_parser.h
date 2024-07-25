#pragma once
#include "slice.h"

typedef struct lx_buf {
  unsigned char *buf;
  size_t size;
} lx_buf_t;

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

typedef enum lx_parser_state {
  // line section
  h_line_start,
  h_method,
  h_method_end,
  h_uri_start,
  h_uri,
  h_uri_end,
  h_version_start,
  h_version_h,
  h_version_ht,
  h_version_htt,
  h_version_http,
  h_version_http_slash,
  h_version_http_major,
  h_version_http_dot,
  h_version_http_minor,
  h_version_end,
  h_line_end,
  // headers section
  h_header_key_start,
  h_header_key,
  h_header_key_end,
  h_header_space,
  h_header_value_start,
  h_header_value,
  h_header_value_end,
  h_header_end,
  h_maybe_end,
  h_end
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
  int content_length_received: 1;
  unsigned nheaders;
} lx_http_parser_t;

const char* lx_http_map_code(lx_parser_status_t);
void lx_http_parser_init(lx_http_parser_t *parser);
int lx_http_parser_exec(lx_http_parser_t *parser, lx_buf_t *data);
