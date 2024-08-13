#include "pandio.h"
#include "poll.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

void pnd_tcp_init(pnd_tcp_t *stream) 
{
  stream->fd = 0;
  stream->writes_size = 0;
  stream->state = PND_TCP_NONE;
  stream->io_handler = NULL;
  stream->data = NULL;
  stream->on_close = NULL;
  stream->on_data = NULL;
  queue_init(&stream->writes);
  pnd_init_event(&stream->ev);
}

/* handler for I/O events from epoll/kqueue */
void pnd_tcp_listener_io(struct pnd_event * event, unsigned events)
{
  assert(events & PND_READABLE);

  pnd_tcp_t *listener = container_of(event, pnd_tcp_t, ev);

  if (listener->state != PND_TCP_ACTIVE) {
    return;
  }

  struct sockaddr_in address;
  int addrlen = sizeof(address);
  pnd_fd_t peer_fd = accept(listener->fd, (struct sockaddr *)&address, &addrlen);
  if (peer_fd < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      return;
    }
  }

  pnd_set_nonblocking(peer_fd);

  if (listener->on_connect != NULL) {
    listener->on_connect(peer_fd);
  }
}

void pnd_tcp_listen(pnd_tcp_t * server, int port, void (*onconnect)(int)) 
{
  pnd_tcp_init(server);

  pnd_fd_t lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lfd <= 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  int opt = 1;

  if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  if (bind(lfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  pnd_set_nonblocking(lfd);

  if (listen(lfd, SOMAXCONN) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  server->fd = lfd;
  server->io_handler = pnd_tcp_listener_io;
  server->state = PND_TCP_ACTIVE;
  server->on_connect = onconnect;

  pnd_add_event(&server->ev, lfd);
}

/* handler for I/O events from epoll/kqueue */
void pnd_tcp_client_io(struct pnd_event * event, unsigned events)
{
  pnd_tcp_t *stream = container_of(event, pnd_tcp_t, ev);

  if (events & PND_READABLE) {
    stream->on_data(stream);
  }

  if (events & PND_CLOSE) {}

  if (events & PND_WRITABLE) {}
}

void pnd_tcp_reject(pnd_fd_t fd) 
{
  int status = 0;

  do {
    status = close(fd);
  } while (status == -1 && errno == EINTR);
}

void pnd_tcp_accept(pnd_tcp_t *peer, pnd_fd_t fd)
{
  pnd_set_nonblocking(fd);
  pnd_tcp_init(peer);
  peer->fd = fd;
  peer->io_handler = pnd_tcp_client_io;
  pnd_add_event(&peer->ev, fd);
}
