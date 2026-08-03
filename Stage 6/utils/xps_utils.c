#include "xps_utils.h"

bool is_valid_port(u_int port) { return port >= 0 && port <= 65535; }

struct addrinfo *xps_getaddrinfo(const char *host, u_int port) {
    assert(host != NULL);
    assert(is_valid_port(port));

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[20];
    sprintf(port_str, "%u", port);

    int err = getaddrinfo(host, port_str, &hints, &result);
    if (err != 0) {
        logger(LOG_ERROR, "xps_getaddrinfo()", "getaddrinfo() error");
        return NULL;
    }

    char ip_str[30];
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
    if (inet_ntop(result->ai_family, &(ipv4->sin_addr), ip_str, sizeof(ip_str)) == NULL) {
        logger(LOG_ERROR, "xps_getaddrinfo()", "inet_ntop() failed");
        perror("Error message");
        freeaddrinfo(result);
        return NULL;
    }

    logger(LOG_DEBUG, "xps_getaddrinfo()", "host: %s, port: %u, resolved ip: %s", host, port, ip_str);

    return result;
}

// int make_socket_non_blocking(u_int sock_fd) {
//     // Get the current socket flags
//     int flags = fcntl(sock_fd, F_GETFL, 0);
//     if (flags < 0) {
//         logger(LOG_ERROR, "make_socket_non_blocking()", "failed to get flags");
//         perror("Error message");
//         return E_FAIL;
//     }

//     // Set flags with O_NONBLOCK
//     if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
//         logger(LOG_ERROR, "make_socket_non_blocking()", "failed to set flags");
//         perror("Error message");
//         return E_FAIL;
//     }

//     return OK;
// }

char *get_remote_ip(u_int sock_fd) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char ipstr[INET_ADDRSTRLEN];

    if (getpeername(sock_fd, (struct sockaddr *)&addr, &addr_len) != 0) {
        logger(LOG_ERROR, "get_remote_ip()", "getpeername() failed");
        perror("Error message");
        return NULL;
    }

    char *ip_str = malloc(INET_ADDRSTRLEN);
    if (ip_str == NULL) {
        logger(LOG_ERROR, "get_remote_ip()", "malloc() failed for 'ip_str");
        return NULL;
    }

    inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);

    return ip_str;
}