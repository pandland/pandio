#include <criterion/criterion.h>

#include "../src/http_request.h"
#include "../src/http_parser.h"

Test(http_parser, parse) {
  http_request_t *req = http_request_alloc();
  int parse_status = http_parse(req, "DELETE /index.html HTTP/1.1\r\n");

  cr_expect_eq(parse_status, 0);
  cr_expect_eq(req->method, DELETE);

  free(req);
}

Test(http_parser, invalid_method) {
  http_request_t *req = http_request_alloc();
  int parse_status = http_parse(req, "DELLTE /index.html HTTP/1.1\r\n");

  cr_expect_eq(parse_status, INVALID_METHOD);

  free(req);
}
