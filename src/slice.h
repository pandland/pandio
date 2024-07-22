#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>

struct buffer_slice {
  const unsigned char *start;
  size_t size;
};

typedef struct buffer_slice slice_t;

static char *slice_to_cstr(slice_t slice) {
    if (slice.size == 0)
      return strdup("");
    char *result = malloc(slice.size + 1);
    memcpy(result, slice.start, slice.size);
    result[slice.size] = '\0';
    
    return result;
}

#define LOWER(c) ((unsigned char)(c) | 0x20)
// lowercase and compare
static bool slice_lower_cstrcmp(slice_t slice, const char* expected) {
    const char *buffer = slice.start;

    while (slice.size-- && *expected) {
        char buf_char = (*buffer >= 'A' && *buffer <= 'Z') ? LOWER(*buffer) : *buffer;
        char str_char = (*expected >= 'A' && *expected <= 'Z') ? LOWER(*expected) : *expected;
        
        if (buf_char != str_char) {
            return false;
        }
        buffer++;
        expected++;
    }
    return *expected == '\0';
}

static int64_t slice_toint64(slice_t slice) {
  const char *buffer = slice.start;
  int64_t result = 0;

  if (slice.size == 0)
    return -1;

  while (slice.size--) {
    if (isdigit((unsigned char)*buffer)) {
      int digit = *buffer - '0';
      if (result > (INT64_MAX - digit) / 10L)
        return -1;

      result = digit + result * 10;
    } else {
      return -1;
    }
    buffer++;
  }

  return result;
}
