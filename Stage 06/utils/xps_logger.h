#ifndef XPS_LOGGER_H
#define XPS_LOGGER_H

#include "../xps.h"

// Basic text colors
#define BLACK_TEXT "\x1b[30m"
#define RED_TEXT "\x1b[31m"
#define GREEN_TEXT "\x1b[32m"
#define YELLOW_TEXT "\x1b[33m"
#define BLUE_TEXT "\x1b[34m"
#define MAGENTA_TEXT "\x1b[35m"
#define CYAN_TEXT "\x1b[36m"
#define WHITE_TEXT "\x1b[37m"

// Extended text colors (256 colors)
#define EXTENDED_TEXT(x) "\x1b[38;5;" #x "m"

// Basic background colors
#define BLACK_BG "\x1b[40m"
#define RED_BG "\x1b[41m"
#define GREEN_BG "\x1b[42m"
#define YELLOW_BG "\x1b[43m"
#define BLUE_BG "\x1b[44m"
#define MAGENTA_BG "\x1b[45m"
#define CYAN_BG "\x1b[46m"
#define WHITE_BG "\x1b[47m"

// Extended background colors (256 colors)
#define EXTENDED_BG(x) "\x1b[48;5;" #x "m"

#define RESET_COLOR "\x1b[0m"

#define BOLD_START "\033[1m"
#define BOLD_END "\033[0m"

typedef enum { LOG_ERROR, LOG_INFO, LOG_DEBUG, LOG_WARNING, LOG_HTTP } xps_log_level_t;

void logger(xps_log_level_t level, const char *function_name, const char *format_string, ...);

#endif