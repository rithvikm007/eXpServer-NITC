#ifndef XPS_BUFFER_H
#define XPS_BUFFER_H

#include "../xps.h"

struct xps_buffer_s {
  size_t size;
  size_t len;
  u_char *pos;
  u_char *data;
};

struct xps_buffer_list_s {
  vec_void_t list;
  size_t len;
};

// xps_buffer
xps_buffer_t *xps_buffer_create(size_t size, size_t len, u_char *data);
void xps_buffer_destroy(xps_buffer_t *buff);
xps_buffer_t *xps_buffer_duplicate(xps_buffer_t *buff);

// xps_buffer_list
xps_buffer_list_t *xps_buffer_list_create();
void xps_buffer_list_destroy(xps_buffer_list_t *buff_list);
void xps_buffer_list_append(xps_buffer_list_t *buff_list, xps_buffer_t *buff);
xps_buffer_t *xps_buffer_list_read(xps_buffer_list_t *buff_list, size_t len);
int xps_buffer_list_clear(xps_buffer_list_t *buff_list, size_t len);

#endif