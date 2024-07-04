#pragma once
#include "stddef.h"

struct buffer_slice {
  const char *start;
  size_t size;
};

typedef struct buffer_slice slice_t;

static char *slice_to_cstr(slice_t slice) {
    char *result = malloc(slice.size + 1);
    memcpy(result, slice.start, slice.size);
    result[slice.size] = '\0';
    
    return result;
}
