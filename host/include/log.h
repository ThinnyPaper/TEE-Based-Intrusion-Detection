
#ifndef _LOG_H_INCLUDED
#define  _LOG_H_INCLUDED

#include <stdarg.h>
#include <stdbool.h>

/* log levels */
typedef enum { /* preserve order */
    LOG_LEVEL_UNSET = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_NOTICE = 3,
    LOG_LEVEL_INFO = 4,
    LOG_LEVEL_RULE = 5,
    LOG_LEVEL_CONFIG = 6,
    LOG_LEVEL_DEBUG = 7,
    LOG_LEVEL_TRACE = 8,
} LOG_LEVEL;

bool is_log_level_unset();

void set_log_level(LOG_LEVEL);

const char * get_log_level_name(LOG_LEVEL);

LOG_LEVEL get_log_level_from_string(char*);

LOG_LEVEL toogle_log_level(LOG_LEVEL);

void log_msg(LOG_LEVEL, const char* ,...);

#define LOG_CONFIG_FORMAT_LINE(log_level, format, ...) \
    if (linebuf) { \
        log_msg(log_level,"%s:%d: " #format " (line: '%s')", filename, linenumber, __VA_ARGS__, linebuf); \
    } else { \
        log_msg(log_level,"%s: " #format, filename, __VA_ARGS__); \
    }

#endif
