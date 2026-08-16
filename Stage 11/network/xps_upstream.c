#include "xps_upstream.h"

xps_connection_t *xps_upstream_create(xps_core_t *core, const char *host, u_int port) {
    /* validate parameter */
    assert(core != NULL);
    assert(host != NULL);

    /* create a socket and connect to host and port to upstream using xps_getaddrinfo and connect function */
    u_int sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(sock_fd < 0) {
        logger(LOG_ERROR, "xps_upstream_create()", "socket() failed");
        return NULL;
    }

    struct addrinfo *addr = xps_getaddrinfo(host, port);
    if(addr == NULL) {
        logger(LOG_ERROR, "xps_upstream_create()", "xps_getaddrinfo() failed");
        close(sock_fd);
        return NULL;
    }

    int connect_error = connect(sock_fd, addr->ai_addr, addr->ai_addrlen);

    if (!(connect_error == 0 || errno == EINPROGRESS)) {
        logger(LOG_ERROR, "xps_upstream_create()", "connect() failed");
        perror("Error message");
        close(sock_fd);
        return NULL;
    }
    free(addr);

    /* create a connection to upstream with core and sock_fd*/
    xps_connection_t *connection = xps_connection_create(core, sock_fd);
    if(connection == NULL) {
        logger(LOG_ERROR, "xps_upstream_create()", "xps_connection_create() failed");
        close(sock_fd);
        return NULL;
    }

    return connection;
}