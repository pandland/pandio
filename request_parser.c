#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "logger.c"

struct {
  char *pos;
  size_t length;
  char *buff;
} typedef RequestParser;

RequestParser init_parser(char *buffer) {
  RequestParser parser;
  parser.pos = buffer;
  parser.length = strlen(buffer);
  parser.buff = buffer;

  return parser;
}

size_t read_line(RequestParser *self, char *line) {
  char *buff = self->pos;
  size_t read_bytes = 0; // it's not a length of the line!

  while (*buff != '\n' && *buff != '\r' && *buff != '\0') {
    *line = *buff;
    line++;
    buff++;
    read_bytes++;
  }

  *line = '\0';

  if (*buff == '\r') {
    buff++;
    read_bytes++;
  }

  if (*buff == '\n') {
    buff++;
    read_bytes++;
  }

  self->pos = buff;

  return read_bytes;
}

void print_request(RequestParser *self) {
  char line[1024];
  read_line(self, line);
  info(line);
}
