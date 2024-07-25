#include "http_parser.h"
#include "http_request.h"
#include "common.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#ifdef __GNUC__
#define LIKELY(X) __builtin_expect(!!(X), 1)
#define UNLIKELY(X) __builtin_expect(!!(X), 0)
#else
#define LIKELY(X) (X)
#define UNLIKELY(X) (X)
#endif

#define CR '\r'
#define LF '\n'

const char *lx_http_map_code(lx_parser_status_t code) {
  switch (code)
  {
  case LX_COMPLETE:
    return "HTTP request is completed and valid.";
  case LX_PARTIAL:
    return "Received partial data and parser is waiting for more data.";
  case LX_UNEXPECTED_ERROR:
    return "Unexpected parser error.";
  case LX_INVALID_METHOD:
    return "HTTP method is invalid.";
  case LX_INVALID_URI:
    return "HTTP URI target is invalid.";
  case LX_INVALID_CHAR:
    return "Illegal character in HTTP request.";
  case LX_INVALID_HEADER_KEY_CHAR:
    return "Illegal character in header key.";
  case LX_INVALID_HEADER_VALUE_CHAR:
    return "Illegal character in header value.";
  case LX_TOO_MANY_HEADERS:
    return "Limit of max headers exceeded.";
  case LX_INVALID_CONTENT_LENGTH:
    return "Invalid content-length header value.";
  case LX_CONTENT_LENGTH_DUPLICATE:
    return "Received duplicated content-length header.";
  default:
    return "Unknown HTTP parser code.";
  }
}

#define CHECK_EOF()                                                            \
  if (buf == buf_end) {                                                        \
    return LX_PARTIAL;                                                            \
  }

#define CHECK_CHAR(ch)                                                         \
  if (*(buf++) != ch) {                                                                                                    \
    return LX_INVALID_CHAR;                                                              \
  }

#define EXPECT_CHAR(ch)                                                        \
  CHECK_EOF()                                                                \
  CHECK_CHAR(ch)

static int HTTP_HEADER_KEY_MAP[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int HTTP_HEADER_VALUE_MAP[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

#define MOVE_TO(val) parser->state = val

#define IS_HEADER_KEY_CHAR(ch) HTTP_HEADER_KEY_MAP[ch] == 1

#define IS_HEADER_VALUE_CHAR(ch) HTTP_HEADER_VALUE_MAP[ch] == 1

void lx_http_parser_init(lx_http_parser_t *parser) {
  parser->state = h_line_start;
  parser->nread = 0;
  parser->nheaders = 0;
  parser->content_length_received = 0;

  parser->header_key.start = NULL;
  parser->header_key.size = 0;

  parser->header_value.start = NULL;
  parser->header_value.size = 0;

  parser->uri.start = NULL;
  parser->uri.size - 0;

  parser->method.start = NULL;
  parser->method.size = 0;
}

#define CONTENT_LENGTH "content-length"
#define CONNECTION "connection"
#define CLOSE "close"
#define UPGRADE "upgrade"

#define TRANSFER_ENCODING "transfer-encoding"

bool lx_is_chunked(slice_t value) {
  enum {
    h_tstart,
    h_tchunked,
    h_ttoken,
  };

  int state = h_tstart;
  const char *buf = value.start;

  while (value.size) {
    switch (state)
    {
    case h_tstart:
      if (*buf == 'c' || *buf == 'C')
        state = h_tchunked;
      else if (*buf == ',')
        state = h_tstart;
      else
        state = h_ttoken; // any other token
      break;
    case h_tchunked:
      if ((value.size >= 6) && (strncasecmp(buf, "hunked", 6) == 0)) {
        buf += 6;
        value.size -= 6;
        if (value.size == 0 || *buf == ',') {
          return true;
        }
      }
      state = h_ttoken;
      break;
    case h_ttoken:
      if (*buf == ',')
        state = h_tstart;
      break;
    default:
      state = h_tstart;
      break;
    }

    buf++;
    value.size--;
  }

  return false;
}

// TODO: it would be more performant to detect specific headers during processing
int lx_on_header_complete(lx_http_parser_t *parser) {
  http_raw_header_t *header = parser->req->raw_headers + parser->nheaders;
  header->key = parser->header_key;
  header->value = parser->header_value;
  parser->nheaders++;

  switch(header->key.size) {
    case 17:
      if (slice_lower_cstrcmp(header->key, TRANSFER_ENCODING)) {
        if (lx_is_chunked(header->value))
          parser->req->chunked = 1;
      }
      break;
    case 14:
      // if transfer-encoding chunked is present - ignore this header at all
      if (!parser->req->chunked && slice_lower_cstrcmp(header->key, CONTENT_LENGTH)) {
        int64_t result = slice_toint64(header->value);
        if (result == -1) {
          return LX_INVALID_CONTENT_LENGTH;
        }

        if (parser->content_length_received) {
          return LX_CONTENT_LENGTH_DUPLICATE;
        }

        parser->req->content_length = result;
        parser->content_length_received = 1;
      }
      break;
    case 10:
      if (slice_lower_cstrcmp(header->key, CONNECTION)) {
        if (slice_lower_startswith(header->value, CLOSE))
          parser->req->persistent = 0;
        else if (slice_lower_startswith(header->value, UPGRADE))
          parser->req->upgrade = 1;
        else
          parser->req->persistent = 1;
      }
      break;
    default:
      return 0;
  }
  return 0;
}

int lx_http_parser_exec(lx_http_parser_t *parser, lx_buf_t *data) {
  unsigned char *buf = data->buf + parser->nread;
  unsigned char *buf_end = data->buf + data->size;

  for (; buf != buf_end; parser->nread = buf - data->buf) {
    switch (parser->state) {
    case h_line_start:
      CHECK_EOF();
      if (UNLIKELY(isspace(*buf))) {
        buf++;
      } else {
        parser->method.start = buf;
        MOVE_TO(h_method);
      }
      break;
    case h_method:
      CHECK_EOF();
      // simplified check, all common methods have letters in range: A - U
      if (LIKELY(*buf >= 'A' && *buf <= 'U')) {
        buf++;
        continue;
      } else if (*buf == ' ') {
        MOVE_TO(h_method_end);
      } else {
        return LX_INVALID_METHOD;
      }
    case h_method_end:
      #define EXPECT_METHOD(expected) strncmp(parser->method.start, expected, method_len) == 0

      unsigned char *method_end = buf;
      size_t method_len = method_end - parser->method.start;

      if (method_len == 3 && EXPECT_METHOD("GET")) {
        parser->req->method = GET;
      } else if (method_len == 3 && EXPECT_METHOD("PUT")) {
        parser->req->method = PUT;
      } else if (method_len == 4 && EXPECT_METHOD("POST")) {
        parser->req->method = POST;
      } else if (method_len == 4 && EXPECT_METHOD("HEAD")) {
        parser->req->method = HEAD;
      } else if (method_len == 5 && EXPECT_METHOD("PATCH")) {
        parser->req->method = PATCH;
      } else if (method_len == 5 && EXPECT_METHOD("TRACE")) {
        parser->req->method = TRACE;
      } else if (method_len == 6 && EXPECT_METHOD("DELETE")) {
        parser->req->method = DELETE;
      } else if (method_len == 7 && EXPECT_METHOD("OPTIONS")) {
        parser->req->method = OPTIONS;
      } else if (method_len == 7 && EXPECT_METHOD("CONNECT")) {
        parser->req->method = CONNECT;
      } else {
        return LX_INVALID_METHOD;
      }

      MOVE_TO(h_uri_start);
      break;
    case h_uri_start:
      CHECK_EOF();
      if (LIKELY(isspace(*buf))) {
        buf++;
      } else {
        parser->uri.start = buf;
        MOVE_TO(h_uri);
      }
      break;
    case h_uri:
      CHECK_EOF();
      // temporary:
      if (isalnum(*buf) || *buf == '/' || *buf == '.' || *buf == '?' || *buf == '&' || *buf == '=' || *buf == '#' || *buf == '_' || *buf == '-') {
        buf++;
      } else if (*buf == ' ') {
        MOVE_TO(h_uri_end);
      } else {
        return LX_INVALID_URI;
      }
      break;
    case h_uri_end:
      parser->uri.size = buf - parser->uri.start;
      parser->req->path = slice_to_cstr(parser->uri);
      MOVE_TO(h_version_start);
      break;
    case h_version_start:
      CHECK_EOF();
      if (LIKELY(isspace(*buf))) {
        buf++;
      } else {
        MOVE_TO(h_version_h);
      }
      break;
    case h_version_h:
      EXPECT_CHAR('H');
      MOVE_TO(h_version_ht);
      break;
    case h_version_ht:
      EXPECT_CHAR('T');
      MOVE_TO(h_version_htt);
      break;
    case h_version_htt:
      EXPECT_CHAR('T');
      MOVE_TO(h_version_http);
      break;
    case h_version_http:
      EXPECT_CHAR('P');
      MOVE_TO(h_version_http_slash);
      break;
    case h_version_http_slash:
      EXPECT_CHAR('/');
      MOVE_TO(h_version_http_major);
      break;
    case h_version_http_major:
      EXPECT_CHAR('1');
      MOVE_TO(h_version_http_dot);
      break;
    case h_version_http_dot:
      EXPECT_CHAR('.');
      MOVE_TO(h_version_http_minor);
      break;
    case h_version_http_minor:
      EXPECT_CHAR('1');
      MOVE_TO(h_line_end);
      break;
    case h_line_end:
      EXPECT_CHAR(CR);
      EXPECT_CHAR(LF);
      MOVE_TO(h_header_key_start);
      break;
    case h_header_key_start:
      if (parser->nheaders >= MAX_HEADERS) {
        return LX_TOO_MANY_HEADERS;
      }

      parser->header_key.start = buf;
      parser->header_key.size = 0;
      MOVE_TO(h_header_key);
      break;
    case h_header_key:
      if (IS_HEADER_KEY_CHAR(*buf)) {
        buf++;
      } else if (*buf == ':') {
        MOVE_TO(h_header_key_end);
      } else {
        return LX_INVALID_HEADER_KEY_CHAR;
      }
      break;
    case h_header_key_end:
      parser->header_key.size = buf - parser->header_key.start;
      // Reject empty header keys. Empty header values are allowed.
      if (parser->header_key.size == 0)
        return LX_INVALID_HEADER_KEY_CHAR;
      buf++;
      MOVE_TO(h_header_space);
      break;
    case h_header_space:
      EXPECT_CHAR(' ');
      MOVE_TO(h_header_value_start);
      break;
    case h_header_value_start:
      parser->header_value.start = buf;
      parser->header_value.size = 0;
      MOVE_TO(h_header_value);
      break;
    case h_header_value:
      if (IS_HEADER_VALUE_CHAR(*buf)) {
        buf++;
      } else if (*buf == CR) {
        MOVE_TO(h_header_value_end);
      } else {
        return LX_INVALID_HEADER_VALUE_CHAR;
      }
      break;
    case h_header_value_end:
      parser->header_value.size = buf - parser->header_value.start;
      int errcode = lx_on_header_complete(parser);
      if (errcode)
        return errcode;

      MOVE_TO(h_header_end);
      break;
    case h_header_end:
      EXPECT_CHAR(CR);
      EXPECT_CHAR(LF);
      MOVE_TO(h_maybe_end);
      break;
    case h_maybe_end:
      if (*buf == CR) {
        buf++;
        EXPECT_CHAR(LF);
        MOVE_TO(h_end);
      } else {
        MOVE_TO(h_header_key_start);
      }
      break;
    case h_end:
      parser->nread = buf - data->buf;
      return LX_COMPLETE;
    default:
      return LX_UNEXPECTED_ERROR;
    }
  }
  
  if (parser->state == h_end) {
    parser->nread = buf - data->buf;
    return LX_COMPLETE;
  }

  return LX_PARTIAL;
}
