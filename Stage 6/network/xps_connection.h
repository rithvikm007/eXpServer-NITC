#ifndef XPS_CONNECTION_H
#define XPS_CONNECTION_H

#include "../xps.h"

struct xps_connection_s {
  int epoll_fd;
  int sock_fd;
  xps_listener_t *listener;
  char *remote_ip;
};

xps_connection_t *xps_connection_create(int epoll_fd, int sock_fd);
void xps_connection_destroy(xps_connection_t *connection);
void xps_connection_read_handler(xps_connection_t *connection);

#endif