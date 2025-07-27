#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

int logger_create(const enum logger_type e_type) {
    p_logger = malloc(sizeof(t_logger));
    if (!p_logger) {
        return -1;
    }
    p_logger->e_type = e_type;
    log_info("Logger successfully initialized", NULL);
    return 0;
}

void log_info(const char *msg, const char *info) {
    if (p_logger->e_type == info || p_logger->e_type == debug) {
        if (info) {
            if (msg) {
                printf(ANSI_COLOR_BLUE "[INFO]: %s (%s)" ANSI_COLOR_BLUE "\n", msg, info);
            } else {
                printf(ANSI_COLOR_BLUE "[INFO]: (%s)" ANSI_COLOR_BLUE "\n", info);
            }
        } else {
            if (msg) {
                printf(ANSI_COLOR_BLUE "[INFO]: %s" ANSI_COLOR_BLUE "\n", msg);
            }
        }
    }
}

void log_debug(const char *msg, const char *info) {
    if (p_logger->e_type == debug) {
        if (info) {
            if (msg) {
                printf(ANSI_COLOR_YELLOW "[DEBUG]: %s (%s)" ANSI_COLOR_YELLOW "\n", msg, info);
            } else {
                printf(ANSI_COLOR_YELLOW "[DEBUG]: (%s)" ANSI_COLOR_YELLOW "\n", info);
            }
        } else {
            if (msg) {
                printf(ANSI_COLOR_YELLOW "[DEBUG]: %s" ANSI_COLOR_YELLOW "\n", msg);
            }
        }
    }
}

void logger_destroy() {
    log_info("Logger successfully destroyed", NULL);
    free(p_logger);
}
