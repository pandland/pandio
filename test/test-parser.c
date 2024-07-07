#include <criterion/criterion.h>
#include <string.h>

#include "../src/http_request.h"
#include "../src/http_parser.h"

Test(http_parser, parse) {
  http_request_t *req = http_request_alloc();
  int parse_status = http_parse(req, "DELETE /index.html HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\n\r\n");
  htable_print(&req->headers);
  printf("Path: %s\n", req->path);
  char *hostname_header = htable_get(&req->headers, "Host");

  cr_expect_eq(parse_status, 0);
  cr_expect_str_eq(hostname_header, "localhost");
  cr_expect_str_eq(req->path, "/index.html");
  cr_expect_eq(req->method, DELETE);

  http_request_free(req);
}

Test(http_parser, invalid_method) {
  http_request_t *req = http_request_alloc();

  int parse_status = http_parse(req, "DELTE /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n");

  cr_expect_eq(parse_status, INVALID_METHOD);

  http_request_free(req);
}


Test(http_parser, invalid_request) {
  printf("Testing http_parser::invalid_request\n");
  fflush(stdout);
  http_request_t *req = http_request_alloc();

  int status = http_parse(req, "GET ASDA103I HTTP/1.1213E");

  cr_expect_neq(status, 0);

  http_request_free(req);
}
