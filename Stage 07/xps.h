#ifndef XPS_H
#define XPS_H

// Header files
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

// 3rd party libraries
#include "lib/vec/vec.h" // https://github.com/rxi/vec

// Constants
#define DEFAULT_BACKLOG 64
#define MAX_EPOLL_EVENTS 32
#define DEFAULT_BUFFER_SIZE 100000 // 100 KB
#define DEFAULT_NULLS_THRESH 32

// Error constants
#define OK 0            // Success
#define E_FAIL -1       // Un-recoverable error
#define E_AGAIN -2      // Try again
#define E_NEXT -3       // Do next
#define E_NOTFOUND -4   // File not found
#define E_PERMISSION -5 // File permission denied
#define E_EOF -6        // End of file reached

// Data types
typedef unsigned char u_char;
typedef unsigned int u_int;
typedef unsigned long u_long;

// Structures
struct xps_core_s;
struct xps_loop_s;
struct xps_listener_s;
struct xps_connection_s;

// Struct typedefs
typedef struct xps_core_s xps_core_t;
typedef struct xps_loop_s xps_loop_t;
typedef struct xps_listener_s xps_listener_t;
typedef struct xps_connection_s xps_connection_t;

// Function typedefs
typedef void (*xps_handler_t)(void *ptr);

// xps headers
#include "core/xps_core.h"
#include "core/xps_loop.h"
#include "network/xps_connection.h"
#include "network/xps_listener.h"
#include "utils/xps_logger.h"
#include "utils/xps_utils.h"

#endif