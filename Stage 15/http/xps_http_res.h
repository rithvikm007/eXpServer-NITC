#ifndef XPS_HTTP_RES_H
#define XPS_HTTP_RES_H

#include "../xps.h"

struct xps_http_res_s {
  char response_line[70];
  vec_void_t headers;
  xps_buffer_t *body;
};

xps_http_res_t *xps_http_res_create(xps_core_t *core, u_int code);
void xps_http_res_destroy(xps_http_res_t *res);
xps_buffer_t *xps_http_res_serialize(xps_http_res_t *res);
void xps_http_res_set_body(xps_http_res_t *http_res, xps_buffer_t *buff);

#endif