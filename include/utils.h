#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Logging levels
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} log_level_t;

// Function declarations
void log_message(log_level_t level, const char *format, ...);
void die(const char *msg);
int file_exists(const char *path);
char *read_file_content(const char *path);

#endif