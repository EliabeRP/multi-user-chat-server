#include "libtslog.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

static FILE *log_file = NULL;
static log_level_t min_level = LOG_DEBUG;
static int log_to_stdout = 0;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static const char *level_to_str(log_level_t lvl) {
    switch (lvl) {
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO:  return "INFO";
        case LOG_WARN:  return "WARN";
        case LOG_ERROR: return "ERROR";
        default:        return "UNK";
    }
}

int log_init(const char *filepath, log_level_t level, int to_stdout) {
    pthread_mutex_lock(&log_mutex);
    if (log_file != NULL) { 
        pthread_mutex_unlock(&log_mutex); 
        return -1; 
    }

    log_file = fopen(filepath, "a");
    if (!log_file) { 
        pthread_mutex_unlock(&log_mutex); 
        return -1; 
    }

    min_level = level;
    log_to_stdout = to_stdout;
    pthread_mutex_unlock(&log_mutex);
    return 0;
}

void log_shutdown() {
    pthread_mutex_lock(&log_mutex);
    if (log_file) fclose(log_file);
    log_file = NULL;
    pthread_mutex_unlock(&log_mutex);
}

/* Função interna: recebe va_list */
static void log_vmsg(log_level_t level, const char *fmt, va_list args) {
    if (level < min_level) return;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);

    char timebuf[64];
    snprintf(timebuf, sizeof(timebuf),
             "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
             tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);

    char msgbuf[1024];
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);

    pthread_mutex_lock(&log_mutex);
    if (log_file) {
        fprintf(log_file, "%s [%s] (tid=%lu) %s\n",
                timebuf, level_to_str(level), (unsigned long)pthread_self(), msgbuf);
        fflush(log_file);
    }
    if (log_to_stdout) {
        printf("%s [%s] (tid=%lu) %s\n",
               timebuf, level_to_str(level), (unsigned long)pthread_self(), msgbuf);
    }
    pthread_mutex_unlock(&log_mutex);
}

void log_msg(log_level_t level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_vmsg(level, fmt, args);
    va_end(args);
}

void log_debug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_vmsg(LOG_DEBUG, fmt, args);
    va_end(args);
}

void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_vmsg(LOG_INFO, fmt, args);
    va_end(args);
}

void log_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_vmsg(LOG_WARN, fmt, args);
    va_end(args);
}

void log_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_vmsg(LOG_ERROR, fmt, args);
    va_end(args);
}
