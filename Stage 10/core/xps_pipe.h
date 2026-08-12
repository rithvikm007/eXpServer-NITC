#ifndef XPS_PIPE_H
#define XPS_PIPE_H

#include "../xps.h"

struct xps_pipe_s {
    xps_core_t *core;
    xps_pipe_source_t *source;
    xps_pipe_sink_t *sink;
    xps_buffer_list_t *buff_list;
    size_t buff_thresh;
};

struct xps_pipe_source_s {
    xps_pipe_t *pipe;
    bool ready;
    bool active;
    xps_handler_t handler_cb;
    xps_handler_t close_cb;
    void *ptr;
};

struct xps_pipe_sink_s {
    xps_pipe_t *pipe;
    bool ready;
    bool active;
    xps_handler_t handler_cb;
    xps_handler_t close_cb;
    void *ptr;
};

/* xps_pipe */
xps_pipe_t *xps_pipe_create(xps_core_t *core, size_t buff_thresh, xps_pipe_source_t *source,
                            xps_pipe_sink_t *sink);
void xps_pipe_destroy(xps_pipe_t *pipe);
bool xps_pipe_is_readable(xps_pipe_t *pipe);
bool xps_pipe_is_writable(xps_pipe_t *pipe);
int xps_pipe_attach_source(xps_pipe_t *pipe, xps_pipe_source_t *source);
int xps_pipe_detach_source(xps_pipe_t *pipe);
int xps_pipe_attach_sink(xps_pipe_t *pipe, xps_pipe_sink_t *sink);
int xps_pipe_detach_sink(xps_pipe_t *pipe);

/* xps_pipe_source */
xps_pipe_source_t *xps_pipe_source_create(void *ptr, xps_handler_t handler_cb,
                                            xps_handler_t close_cb);
void xps_pipe_source_destroy(xps_pipe_source_t *source);
int xps_pipe_source_write(xps_pipe_source_t *source, xps_buffer_t *buff);

/* xps_pipe_sink */
xps_pipe_sink_t *xps_pipe_sink_create(void *ptr, xps_handler_t handler_cb, xps_handler_t close_cb);
void xps_pipe_sink_destroy(xps_pipe_sink_t *sink);
xps_buffer_t *xps_pipe_sink_read(xps_pipe_sink_t *sink, size_t len);
int xps_pipe_sink_clear(xps_pipe_sink_t *sink, size_t len);

#endif