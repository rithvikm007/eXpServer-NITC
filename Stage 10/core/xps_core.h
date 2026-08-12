#ifndef XPS_CORE_H
#define XPS_CORE_H

#include "../xps.h"

struct xps_core_s {
  xps_loop_t *loop;
  vec_void_t listeners;
  vec_void_t connections;
  vec_void_t pipes;
  u_int n_null_listeners;
  u_int n_null_connections;
  u_int n_null_pipes;
};

xps_core_t *xps_core_create();
void xps_core_destroy(xps_core_t *core);
void xps_core_start(xps_core_t *core);

#endif