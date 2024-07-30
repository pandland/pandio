#pragma once
#include "event.h"
#include "net.h"
#include "http_request.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*lx_http_onrequest)();

typedef struct lx_http {
  int port;                       // port to listen
  int timeout;                    // absolute connection timeout
  lx_http_onrequest handler;      // handles actual HTTP request
  void *data;                     // user data
} lx_http_t;

lx_http_t lx_http_init(int port, lx_http_onrequest handler); 
void lx_http_listen(lx_io_t*, lx_http_t*);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
