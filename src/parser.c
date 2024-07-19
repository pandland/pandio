#include <ctype.h>
#include <string.h>

#include "common.h"
#include "http_request.h"
#include "slice.h"

#define MAX_HEADERS 64

#ifdef __GNUC__
#define LIKELY(X) __builtin_expect(!!(X), 1)
#define UNLIKELY(X) __builtin_expect(!!(X), 0)
#else
#define LIKELY(X) (X)
#define UNLIKELY(X) (X)
#endif

#define CR '\r'
#define LF '\n'

typedef struct lxe_buf {
  const unsigned char *buf;
  size_t size;
} lxe_buf_t;

typedef enum lxe_parser_status { error, partial, complete } lxe_parser_status_t;

typedef enum lxe_parser_state {
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
  end,
  parser_err
} lxe_parser_state_t;

typedef struct lxe_parser {
  size_t nread;
  lxe_parser_state_t state;
  http_request_t *req;
  slice_t uri;
  slice_t method;
  slice_t header_key;
  slice_t header_value;
} lxe_parser_t;

#define CHECK_EOF()                                                            \
  if (buf == buf_end) {                                                        \
    return partial;                                                            \
  }

#define CHECK_CHAR(ch)                                                         \
  if (*(buf++) != ch) {                                                           \
    parser->state = parser_err;                                                \
    return error;                                                              \
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

static void parser_init(lxe_parser_t *parser) {
  parser->state = line_start;
  parser->nread = 0;
  parser->header_key.start = NULL;
  parser->header_key.size = 0;

  parser->header_value.start = NULL;
  parser->header_value.size = 0;

  parser->uri.start = NULL;
  parser->uri.size - 0;

  parser->method.start = NULL;
  parser->method.size = 0;
}

static int parse(lxe_parser_t *parser, lxe_buf_t *data) {
  unsigned char *buf = data->buf + parser->nread;
  unsigned char *buf_end = data->buf + data->size;

  for (; buf != buf_end; parser->nread = buf - data->buf) {
    switch (parser->state) {
    case line_start:
      CHECK_EOF();
      if (UNLIKELY(isspace(*buf))) {
        buf++;
      } else {
        parser->method.start = buf;
        MOVE_TO(method);
      }
      break;
    case method:
      CHECK_EOF();
      // simplified check, all common methods have letters in range: A - U
      if (LIKELY(*buf >= 'A' && *buf <= 'U')) {
        buf++;
        continue;
      } else if (*buf == ' ') {
        MOVE_TO(method_end);
      } else {
        return error;
      }
    case method_end:
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
      } else if (method_len == 5 && EXPECT_METHOD("TRACE")) {
        parser->req->method = TRACE;
      } else if (method_len == 6 && EXPECT_METHOD("DELETE")) {
        parser->req->method = DELETE;
      } else if (method_len == 7 && EXPECT_METHOD("OPTIONS")) {
        parser->req->method = OPTIONS;
      } else if (method_len == 7 && EXPECT_METHOD("CONNECT")) {
        parser->req->method = CONNECT;
      } else {
        return error;
      }

      MOVE_TO(uri_start);
      break;
    case uri_start:
      CHECK_EOF();
      if (LIKELY(isspace(*buf))) {
        buf++;
      } else {
        parser->uri.start = buf;
        MOVE_TO(uri);
      }
      break;
    case uri:
      CHECK_EOF();
      // temporary:
      if (isalnum(*buf) || *buf == '/' || *buf == '.' || *buf == '?' || *buf == '&' || *buf == '#') {
        buf++;
      } else if (*buf == ' ') {
        MOVE_TO(uri_end);
      } else {
        return error;
      }
      break;
    case uri_end:
      parser->uri.size = buf - parser->uri.start;
      MOVE_TO(version_start);
      break;
    case version_start:
      CHECK_EOF();
      if (LIKELY(isspace(*buf))) {
        buf++;
      } else {
        MOVE_TO(version_h);
      }
      break;
    case version_h:
      EXPECT_CHAR('H');
      MOVE_TO(version_ht);
      break;
    case version_ht:
      EXPECT_CHAR('T');
      MOVE_TO(version_htt);
      break;
    case version_htt:
      EXPECT_CHAR('T');
      MOVE_TO(version_http);
      break;
    case version_http:
      EXPECT_CHAR('P');
      MOVE_TO(version_http_slash);
      break;
    case version_http_slash:
      EXPECT_CHAR('/');
      MOVE_TO(version_http_major);
      break;
    case version_http_major:
      EXPECT_CHAR('1');
      MOVE_TO(version_http_dot);
      break;
    case version_http_dot:
      EXPECT_CHAR('.');
      MOVE_TO(version_http_minor);
      break;
    case version_http_minor:
      EXPECT_CHAR('1');
      MOVE_TO(line_end);
      break;
    case line_end:
      EXPECT_CHAR(CR);
      EXPECT_CHAR(LF);
      MOVE_TO(header_key_start);
      break;
    case header_key_start:
      parser->header_key.start = buf;
      parser->header_key.size = 0;
      MOVE_TO(header_key);
      break;
    case header_key:
      if (IS_HEADER_KEY_CHAR(*buf)) {
        buf++;
      } else if (*buf == ':') {
        MOVE_TO(header_key_end);
      } else {
        return error;
      }
      break;
    case header_key_end:
      parser->header_key.size = buf - parser->header_key.start;
      buf++;
      MOVE_TO(header_space);
      break;
    case header_space:
      EXPECT_CHAR(' ');
      MOVE_TO(header_value_start);
      break;
    case header_value_start:
      parser->header_value.start = buf;
      parser->header_value.size = 0;
      MOVE_TO(header_value);
      break;
    case header_value:
      if (IS_HEADER_VALUE_CHAR(*buf)) {
        buf++;
      } else if (*buf == CR) {
        MOVE_TO(header_value_end);
      } else {
        return error;
      }
      break;
    case header_value_end:
      parser->header_value.size = buf - parser->header_value.start;
      MOVE_TO(header_end);
      break;
    case header_end:
      EXPECT_CHAR(CR);
      EXPECT_CHAR(LF);
      MOVE_TO(maybe_end);
      break;
    case maybe_end:
      if (*buf == CR) {
        buf++;
        EXPECT_CHAR(LF);
        MOVE_TO(end);
      } else {
        MOVE_TO(header_key_start);
      }
      break;
    case end:
      return complete;
    }
  }
  
  if (parser->state == end) {
    return complete;
  }

  return partial;
}

int main() { 
  lxe_parser_t parser;
  parser_init(&parser);

  http_request_t *r = http_request_alloc();
  parser.req = r;

  unsigned char str[] = "PUT /index.html HTTP/1.1\r\nHost: localhost\r\nX-User: Michał\r\n\r\n";
  //unsigned char *str1 = "PUT ";
  //strcpy(str, str1);
  lxe_buf_t buf = { .buf = str, .size = 50 };
  //int result0 = parse_line(&parser, &buf);
  //printf("Result of partial is: %d\n", result0);

  //unsigned char *str2 = "/index.html ";
  //strcpy(str + 2, str2);
  //buf.size += 15;
  parse(&parser, &buf);
  printf("Parsed: %ld\n", parser.nread);
  buf.size = strlen(str);
  int result = parse(&parser, &buf);

  printf("Nread: %ld vs actual size: %ld\n", parser.nread, buf.size);
  printf("Method: %s\n", map_method(r->method));
  printf("Result is: %d\n", result);
  printf("State is: %d\n", parser.state);
  printf("Path: %s\n", slice_to_cstr(parser.uri));
  printf("Last header is '%s': '%s'\n", slice_to_cstr(parser.header_key), slice_to_cstr(parser.header_value));
}
