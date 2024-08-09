#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// stolen from linux kernel source code
#define container_of(ptr, type, member) ({ \
    const __typeof__(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); })

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
