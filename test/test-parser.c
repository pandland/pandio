#include <criterion/criterion.h>

#include "../src/http_request.h"
#include "../src/http_parser.h"

Test(http_parser, parse) {
  http_request_t *req = http_request_alloc();
  int parse_status = http_parse(req, "DELETE /index.html HTTP/1.0");

  cr_expect_eq(parse_status, 0);
  cr_expect_eq(req->method, DELETE);

  free(req);
}
