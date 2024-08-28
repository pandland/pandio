#include <pandio.h>
#include <stdio.h>
#include <string.h>

void handle_write(pd_write_t *write_op, int status) {
    printf("Written with status: %d\n", status);
    free(write_op->data.buf);
    free(write_op);
}

void handle_close(pd_tcp_t *stream) {
    printf("Connection closed\n");
    free(stream);
}

void handle_read(pd_tcp_t *stream, char *buf, size_t len) {
    printf("Received data with len: %zu\n", len);
    printf("%.*s\n", (int)len, buf);
    pd_tcp_close(stream);
}

void handle_connect(pd_tcp_t *stream) {
    printf("Connected.\n");
    const char *req = "Hello World";
    pd_write_t *write_op = malloc(sizeof(pd_write_t));
    pd_write_init(write_op, strdup(req), strlen(req), handle_write);

    stream->on_close = handle_close;
    stream->on_data = handle_read;
    pd_tcp_write(stream, write_op);
}

int main() {
    pd_io_t *ctx = malloc(sizeof(pd_io_t));
    pd_io_init(ctx);

    pd_tcp_t *stream = malloc(sizeof(pd_tcp_t));
    pd_tcp_init(ctx, stream);
    printf("Connecting to server...\n");
    pd_tcp_connect(stream, "127.0.0.1", 8000, handle_connect);

    pd_io_run(ctx);
    return 0;
}
