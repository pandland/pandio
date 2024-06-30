#pragma once
#include "string.h"
#include "http_request.h"

enum HTTP_PARSER_STATE {
  METHOD,
  PATH,
  VERSION,
  HEADERS_START,
  HEADER_NAME,
  HEADER_VALUE,
  BODY_START,
  ERROR
};

#define SET_ERROR() \
  parser->state = ERROR

#define EXPECT_CHAR(ch) \
  if (*(buf++) != ch) {     \
    SET_ERROR();         \
    return;        \
  }                     \


typedef struct http_parser {
  enum HTTP_PARSER_STATE state;
  http_request_t *req;
} http_parser_t;

static void parse_method(http_parser_t *parser, char *buf) {
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
  parser->state = PATH;
}

static int http_parse(http_request_t *req, char *buf) {
  http_parser_t parser = { .state = METHOD, .req = req };

  while (*buf) {
    switch (parser.state) {
      case ERROR:
        return -1;
      case METHOD:
        parse_method(&parser, buf);
        break;
    }

    // for now...
    buf++;
  }

  return 0;
}
