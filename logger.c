#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRETTY_LOGGER 1

enum {
  RED = 41,
  GREEN = 42,
  YELLOW = 43,
} typedef Color;

char *colorful(const char *str, const Color color) {
  if (!PRETTY_LOGGER) {
    return strdup(str);
  }
  char* buff = malloc(strlen(str) + 16);
  sprintf(buff, "%s;%dm%s%s", "\x1b[30", color, str, "\x1b[0m");

  return buff;
}

void die(const char *reason) {
  char *prefix = colorful(" CRITICAL ", RED);
  printf("%s %s\n", prefix, reason);
  free(prefix);
  exit(EXIT_FAILURE);
}

void warn(const char *reason) {
  char *prefix = colorful(" WARNING ", YELLOW);
  printf("%s %s\n", prefix, reason);
  free(prefix);
}

void info(const char *reason) {
  char *prefix = colorful(" INFO ", GREEN);
  printf("%s %s\n", prefix, reason);
  free(prefix);
}
