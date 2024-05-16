enum {
  GET,
  POST,
  PUT,
  PATCH,
  OPTIONS,
  DELETE
} typedef Method;

struct {
  Method method;
  char *path;
  char *version;
} typedef Request;

Request parse_request(const char *buffer) {
  Request request;
  request.method = GET;
  request.path = "/";
  request.version = "HTTP/1.1";

  return request;
}
