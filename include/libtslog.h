#ifndef LIBTSLOG_H
#define LIBTSLOG_H

#include <stdio.h>
#include <pthread.h>

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} log_level_t;

int log_init(const char *filepath, log_level_t min_level, int to_stdout);
void log_shutdown();
void log_msg(log_level_t level, const char *fmt, ...);

void log_debug(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_error(const char *fmt, ...);

#endif
