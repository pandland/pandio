#include "pandio.h"

void pnd_poll_init(pnd_io_t * ctx);

void pnd_poll_run(pnd_io_t * ctx, int timeout);

void pnd_init_event(pnd_event_t * event);

void pnd_modify_event(pnd_event_t * event, int fd, uint32_t operation, uint32_t flags);

void pnd_add_event(pnd_event_t * event, pnd_fd_t fd);

void pnd_start_reading(pnd_event_t * event, pnd_fd_t fd);

void pnd_stop_reading(pnd_event_t * event, pnd_fd_t fd);

void pnd_start_writing(pnd_event_t * event, pnd_fd_t fd);

void pnd_stop_writing(pnd_event_t * event, pnd_fd_t fd);

void pnd_remove_event(pnd_event_t * event, pnd_fd_t fd);

void pnd_set_nonblocking(pnd_fd_t fd);
