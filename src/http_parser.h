#pragma once
#include "string.h"
#include "ctype.h"
#include "http_request.h"
#include "stdio.h"

enum HTTP_PARSER_STATE {
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
  }                     \


typedef struct http_parser {
  enum HTTP_PARSER_STATE state;
  http_request_t *req;
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
    default:
      SET_ERROR();
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

static int http_parse(http_request_t *req, char *buf) {
  http_parser_t parser = { .state = METHOD, .req = req };

  while (*buf) {
    //printf("buffer value: %s\n", buf);
    switch (parser.state) {
      case METHOD:
        buf = parse_method(&parser, buf);
        break;
      case URL:
        buf = parse_url(&parser, buf);
        break;
      case VERSION:
        buf = parse_version(&parser, buf);
        break;
      case HEADERS_START:
        return 0;
    }

    if (parser.state == ERROR) {
      return -1;
    }
  }

  return 0;
}
