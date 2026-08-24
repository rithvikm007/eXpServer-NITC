#ifndef XPS_HTTP_REQ_H
#define XPS_HTTP_REQ_H

#include "xps_http.h"

struct xps_http_req_s {
  xps_http_parser_state_t parser_state;

  xps_http_method_t method_n;

  char *request_line; // eg: POST https://www.devdiary.live:3000/api/problems HTTP/1.1
  u_char *request_line_start;
  u_char *request_line_end;

  char *method; // eg: POST
  u_char *method_start;
  u_char *method_end;

  char *uri; // eg: https://www.devdiary.live:3000/api/problems?key=val
  u_char *uri_start;
  u_char *uri_end;

  char *schema; // eg: https
  u_char *schema_start;
  u_char *schema_end;

  char *host; // eg: www.devdiary.live
  u_char *host_start;
  u_char *host_end;

  int port; // eg: 3000
  u_char *port_start;
  u_char *port_end;

  char *path; // eg: /api/problems?key=val
  u_char *path_start;
  u_char *path_end;

  char *pathname; // eg: /api/problems
  u_char *pathname_start;
  u_char *pathname_end;

  char *http_version; // eg: 1.1
  u_char *http_major;
  u_char *http_minor;

  vec_void_t headers;

  u_char *header_key_start;
  u_char *header_key_end;
  u_char *header_val_start;
  u_char *header_val_end;

  size_t header_len;
  size_t body_len;
};

xps_http_req_t *xps_http_req_create(xps_core_t *core, xps_buffer_t *buff, int *error);
void xps_http_req_destroy(xps_core_t *core, xps_http_req_t *http_req);
xps_buffer_t *xps_http_req_serialize(xps_http_req_t *http_req);

#endif