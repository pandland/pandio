#pragma once
#include "string.h"
#include "ctype.h"
#include "http_request.h"
#include "stdio.h"
#include "slice.h"

#define MAX_HEADERS 64

enum http_parser_state {
  METHOD,
  URL,
  VERSION,
  HEADERS_START,
  HEADER_NAME,
  HEADER_VALUE,
  BODY_START,
  ERROR
};

#define CR '\r'
#define LF '\n'

#define SET_ERROR() \
  parser->state = ERROR

#define EXPECT_CHAR(ch) \
  if (*(buf++) != ch) {     \
    SET_ERROR();         \
    return NULL;        \
  }

#define SET_ERRNO(no) \
  if (parser.state == ERROR) \
    parser.err = no

enum http_parser_errno {
  UNKNOWN = 1,
  INVALID_METHOD,
  INVALID_PATH,
  INVALID_VERSION,
  INVALID_HEADER
};

typedef struct http_parser {
  enum http_parser_state state;
  enum http_parser_errno err;
  http_request_t *req;
  slice_t header_name;
  slice_t header_value;
} http_parser_t;

static char *parse_method(http_parser_t *parser, char *buf) {
  switch (*buf) {
    case 'G':
      EXPECT_CHAR('G');
      EXPECT_CHAR('E');
      EXPECT_CHAR('T');
      parser->req->method = GET;
      break;
    case 'P':
      EXPECT_CHAR('P');
      if (*buf == 'A') {
        EXPECT_CHAR('A');
        EXPECT_CHAR('T');
        EXPECT_CHAR('C');
        EXPECT_CHAR('H');
        parser->req->method = PATCH;
      }
      else if (*buf == 'O') {
        EXPECT_CHAR('O');
        EXPECT_CHAR('S');
        EXPECT_CHAR('T');
        parser->req->method = POST;
      } else {
        EXPECT_CHAR('U');
        EXPECT_CHAR('T');
        parser->req->method = PUT;
      }
      break;
    case 'D':
      EXPECT_CHAR('D');
      EXPECT_CHAR('E');
      EXPECT_CHAR('L');
      EXPECT_CHAR('E');
      EXPECT_CHAR('T');
      EXPECT_CHAR('E');
      parser->req->method = DELETE;
      break;
    case 'O':
      EXPECT_CHAR('O');
      EXPECT_CHAR('P');
      EXPECT_CHAR('T');
      EXPECT_CHAR('I');
      EXPECT_CHAR('O');
      EXPECT_CHAR('N');
      EXPECT_CHAR('S');
      parser->req->method = OPTIONS;
    default:
      SET_ERROR();
      return NULL;
  }

  EXPECT_CHAR(' ');
  parser->state = URL;

  return buf;
}

static char *parse_url(http_parser_t *parser, char *buf) {
  // temporary
  while (*buf && *buf != ' ') {
    if (!isalnum(*buf) && *buf != '/' && *buf != '.') {
      SET_ERROR();
      return NULL;
    }

    buf++;
  }

  parser->state = VERSION;
  EXPECT_CHAR(' ');
  return buf;
}

static char *parse_version(http_parser_t *parser, char *buf) {
  EXPECT_CHAR('H');
  EXPECT_CHAR('T');
  EXPECT_CHAR('T');
  EXPECT_CHAR('P');
  EXPECT_CHAR('/');
  EXPECT_CHAR('1');
  EXPECT_CHAR('.');
  EXPECT_CHAR('1');

  EXPECT_CHAR(CR);
  EXPECT_CHAR(LF);

  parser->state = HEADERS_START;

  return buf;
}

static char *parse_header_name(http_parser_t *parser, char *buf) {
  if (buf == NULL) {
    fprintf(stderr, "Error: NULL buffer\n");
    return NULL;
  }

  char *header_start = buf;
  size_t header_size = 0;

  while (*buf) {
    if (*buf == ':') {
      break;
    }
    header_size++;
    buf++;
  }

  parser->state = HEADER_VALUE;

  parser->header_name.start = header_start;
  parser->header_name.size = header_size;

  EXPECT_CHAR(':');
  EXPECT_CHAR(' ');

  return buf;
}

static char *parse_header_value(http_parser_t *parser, char *buf) {
  if (buf == NULL) {
    fprintf(stderr, "Error: NULL buffer\n");
    return NULL;
  }

  char *header_start = buf;
  size_t header_size = 0;

  while (*buf) {
    if (*buf == CR) {
      break;
    }
    header_size++;
    buf++;
  }

  parser->state = HEADER_VALUE;

  parser->header_value.start = header_start;
  parser->header_value.size = header_size;

  EXPECT_CHAR(CR);
  EXPECT_CHAR(LF);

  return buf;
}

/* returns non-zero value on error */
static int http_parse(http_request_t *req, char *buf) {
  http_parser_t parser = { .state = METHOD, .req = req, .err = UNKNOWN };

  while (*buf) {
    //printf("buffer value: %s\n", buf);
    switch (parser.state) {
      case METHOD:
        buf = parse_method(&parser, buf);
        SET_ERRNO(INVALID_METHOD);
        break;
      case URL:
        buf = parse_url(&parser, buf);
        SET_ERRNO(INVALID_PATH);
        break;
      case VERSION:
        buf = parse_version(&parser, buf);
        SET_ERRNO(INVALID_VERSION);
        break;
      case HEADERS_START:
        parser.state = HEADER_NAME;
        break;
      case HEADER_NAME:
        buf = parse_header_name(&parser, buf);
        SET_ERRNO(INVALID_HEADER);
        break;
      case HEADER_VALUE:
        buf = parse_header_value(&parser, buf);
        SET_ERRNO(INVALID_HEADER);

        char *header_name = slice_to_cstr(parser.header_name);
        char *header_value = slice_to_cstr(parser.header_value);

        htable_insert(&parser.req->headers, header_name, header_value);
        //htable_print(&parser.req->headers);

        return 0;
    }

    if (parser.state == ERROR) {
      return parser.err;
    }
  }

  return 0;
}
