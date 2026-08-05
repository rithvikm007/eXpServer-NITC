#ifndef XPS_LOOP_H
#define XPS_LOOP_H

#include "../xps.h"

struct xps_loop_s {
  xps_core_t *core;
  u_int epoll_fd;
  struct epoll_event epoll_events[MAX_EPOLL_EVENTS];
  vec_void_t events;
  u_int n_null_events;
};

struct loop_event_s {
  u_int fd;
  xps_handler_t read_cb;
  void *ptr;
};

typedef struct loop_event_s loop_event_t;

xps_loop_t *xps_loop_create(xps_core_t *core);
void xps_loop_destroy(xps_loop_t *loop);
int xps_loop_attach(xps_loop_t *loop, u_int fd, int event_flags, void *ptr, xps_handler_t read_cb);
int xps_loop_detach(xps_loop_t *loop, u_int fd);
void xps_loop_run(xps_loop_t *loop);

#endif