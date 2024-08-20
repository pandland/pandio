/* Common internal functions / helpers */
#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
