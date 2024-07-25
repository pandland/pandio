#include "http.h"
#include "logger.h"

#define HTTP_DEFAULT_TIMEOUT 60000
#define lx_http_ctx(conn) conn->listener->data

void print_raw_headers(http_request_t *req) {
  unsigned nheaders = req->parser.nheaders;

  printf("{\n");
  for (int i = 0; i < nheaders; ++i) {
    http_raw_header_t header = req->raw_headers[i];
    char *header_key = slice_to_cstr(header.key);
    char *header_value = slice_to_cstr(header.value);
    printf("  %s: %s, \n", header_key, header_value);
    free(header_key);
    free(header_value);
  }
  printf("}\n");
}

// handle request with parsed headers
void lx_http_on_request(http_request_t *req) {
  log_info("%s %s", http_map_method(req->method), req->path);
  print_raw_headers(req);
  
  if (req->body) {
    log_info("Request with body (%ld):", req->received);
    //fwrite(req->body, 1, req->received + 1, stdout);
    for (size_t i = 0; i < req->received; ++i) {
      printf("%c", req->body[i]);
    }
    printf("\n");
  }

  const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 22\r\nContent-Type: text/html\r\n\r\n<h1>Hello world!</h1>\n";
  send(req->connection->fd, response, strlen(response), 0);
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MAX_BODY_BUFFER 16000

void lx_http_read_body(lx_connection_t *conn) {
  lx_http_t *http_ctx = lx_http_ctx(conn);
  http_request_t *req = conn->data;

  size_t to_read = req->content_length - req->received;

  if (to_read <= 0) {
    lx_http_on_request(req);
    lx_close(conn);
    return;
  }

  size_t chunk_size = recv(conn->fd, req->body + req->received, to_read, 0);
  if (chunk_size < 0) {
    perror("body:recv");
    lx_close(conn);
    return;
  }

  if (chunk_size == 0) {
    lx_close(conn);
    return;
  }

  req->received += chunk_size;

  if (req->content_length <= req->received) {
    lx_http_on_request(req);
    lx_close(conn);
  }
}

void lx_http_read_headers(lx_connection_t *conn) {
  lx_http_t *http_ctx = lx_http_ctx(conn);
  http_request_t *req = conn->data;

  // reading headers:
  char *buf = conn->buf + conn->size;
  size_t to_read = LX_NET_BUFFER_SIZE - conn->size;
  if (to_read == 0 && req->parser.state != h_end) {
    log_err("Max headers size exceeded.");
    lx_close(conn);
    return;
  }

  int bytes = recv(conn->fd, buf, to_read, 0);
  conn->size += bytes;

  if (bytes < 0) {
    perror("recv");
    lx_close(conn);
    return;
  }

  if (bytes == 0) {
    lx_close(conn);
    return;
  }

  lx_buf_t input = { .buf = conn->buf, .size = conn->size };
  int parser_code = lx_http_parser_exec(&req->parser, &input);

  switch (parser_code) {
    case LX_COMPLETE:
      size_t nread = req->parser.nread;
      if (req->content_length == 0) {
        lx_http_on_request(req);
        lx_close(conn);
        return;
      }

      if (req->content_length > MAX_BODY_BUFFER) {
        log_err("Too big body");
        lx_close(conn);
        return;
      }

      req->body = malloc(req->content_length * sizeof(char));
      if (conn->size > nread) {
        memcpy(req->body, conn->buf + nread, conn->size - nread);
        req->received += conn->size - nread;
      }

      if (req->received == req->content_length) {
        lx_http_on_request(req);
        lx_close(conn);
        return;
      }

      conn->ondata = lx_http_read_body;
      return;
    case LX_PARTIAL:
      log_info("Received partial data");
      return;
    default:
      log_err("Parsing failure: %s", lx_http_map_code(parser_code));
      lx_close(conn);
      return;
  }
}

// handle conn->onclose()
void lx_http_handle_close(lx_connection_t *conn) {
    http_request_t *req = conn->data;
    lx_timer_stop(&req->timeout);
    http_request_free(req);
}

void lx_http_handle_timeout(lx_timer_t *timeout) {
    lx_connection_t *conn = timeout->data;
    lx_close(conn);
    log_warn("Connection timed out");
}

// handle listener->onaccept()
void lx_http_handle_accept(lx_connection_t *conn) {
  lx_io_t *ctx = lx_conn_ctx(conn); 
  http_request_t *req = http_request_alloc();

  lx_timer_init(ctx, &req->timeout);
  req->connection = conn;
  req->timeout.data = conn;
  
  conn->data = req;
  conn->ondata = lx_http_read_headers;
  conn->onclose = lx_http_handle_close;

  lx_http_t *http_ctx = lx_http_ctx(conn);
  lx_timer_start(&req->timeout, lx_http_handle_timeout, http_ctx->timeout);
}

void lx_http_listen(lx_io_t* ctx, lx_http_t *http) {
  lx_listener_t *listener = lx_listen(ctx, http->port, lx_http_handle_accept);
  listener->data = http;
}

lx_http_t lx_http_init(int port, lx_http_onrequest handler) {
  lx_http_t http = { .port = port, .timeout = HTTP_DEFAULT_TIMEOUT, .handler = handler };
  return http;
}
