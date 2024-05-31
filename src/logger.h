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

char *colorful(const char*, const Color);
void die(const char*);
void warn(const char*);
void info(const char*);
