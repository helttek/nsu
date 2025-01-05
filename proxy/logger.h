#ifndef LOGGER_H
#define LOGGER_H

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RED "\x1b[31m"

enum logger_type {
    debug = 0,
    info = 1,
};

typedef struct logger {
    enum logger_type e_type;
} t_logger;

static t_logger *p_logger;

int logger_create(enum logger_type e_type);

void log_info(const char *msg, const char *info);

void log_debug(const char *msg, const char *info);

void logger_destroy();

#endif // LOGGER_H
