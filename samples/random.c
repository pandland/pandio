#include <stdio.h>
#include <pandio.h>

#define n 16

int main() {
    unsigned char buf[n];
    int status = pd_random(buf, 16);

    if (status < 0) {
        printf("error: %s\n", pd_errstr(status));
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");

    return 0;
}
