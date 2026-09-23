#ifndef XPS_UTILS_H
#define XPS_UTILS_H

#include "../xps.h"

// Sockets
bool is_valid_port(u_int port);
int make_socket_non_blocking(u_int sock_fd);
struct addrinfo *xps_getaddrinfo(const char *host, u_int port);
char *get_remote_ip(u_int sock_fd);
// Other functions
void vec_filter_null(vec_void_t *v);
const char *get_file_ext(const char *file_path);
char *str_from_ptrs(const char *start, const char *end);
bool str_starts_with(const char *str, const char *prefix);
char *path_join(const char *path_1, const char *path_2);
char *str_create(const char *str);
bool is_dir(const char *path);
bool is_file(const char *path);
bool is_abs_path(const char *path);

#endif