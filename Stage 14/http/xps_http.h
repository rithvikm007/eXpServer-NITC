#ifndef XPS_HTTP_H
#define XPS_HTTP_H

#include "../xps.h"

#define CR '\r'
#define LF '\n'

int xps_http_parse_request_line(xps_http_req_t *http_req, xps_buffer_t *buffer);
int xps_http_parse_header_line(xps_http_req_t *http_req, xps_buffer_t *buffer);

const char *xps_http_get_header(vec_void_t *headers, const char *key);
xps_buffer_t *xps_http_serialize_headers(vec_void_t *headers);

#endif