#ifndef XPS_UPSTREAM_H
#define XPS_UPSTREAM_H

#include "../xps.h"

xps_connection_t *xps_upstream_create(xps_core_t *core, const char *host, u_int port);

#endif