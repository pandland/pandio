#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PRETTY_LOG
#define PRETTY_LOG 1
#endif

#define RESET_COLOR  "\x1b[0m"
#define RED_COLOR    "\x1b[31m"
#define YELLOW_COLOR "\x1b[33m"
#define GREEN_COLOR  "\x1b[32m"

#if PRETTY_LOG
    #define log_info(fmt, ...)  printf(GREEN_COLOR "[INFO] " RESET_COLOR fmt "\n", ##__VA_ARGS__)
    #define log_warn(fmt, ...)  printf(YELLOW_COLOR "[WARN] " RESET_COLOR fmt "\n", ##__VA_ARGS__)
    #define log_err(fmt, ...)  fprintf(stderr, RED_COLOR "[ERROR] " RESET_COLOR  fmt "\n", ##__VA_ARGS__)
#else
    #define log_info(fmt, ...)  printf("[INFO] " fmt "\n", ##__VA_ARGS__)
    #define log_warn(fmt, ...)  printf("[WARN] " fmt "\n", ##__VA_ARGS__)
    #define log_err(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)
#endif

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
