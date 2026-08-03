#include "xps.h"

// Global variables
int epoll_fd;
struct epoll_event events[MAX_EPOLL_EVENTS];
vec_void_t listeners;
vec_void_t connections;

int main() {

    epoll_fd = xps_loop_create();

    // Init lists
    vec_init(&listeners);
    vec_init(&connections);

    // Create listeners on ports 8001, 8002, 8003
    for (int port = 8001; port <= 8004; port++) {
        /* create listener instance using xps_listener_create() */
        xps_listener_create(epoll_fd, "0.0.0.0", port);
        logger(LOG_INFO, "main()", "Server listening on port %u", port);
    }

    /* run the event loop using xps_loop_run() */
    xps_loop_run(epoll_fd);
}

int xps_loop_create() {
    /* create a loop instance and return epoll FD */
    int epoll_fd = epoll_create1(0);
    return epoll_fd;
}

void xps_loop_attach(int epoll_fd, int fd, int events) {
    /* attach fd to epoll */
    struct epoll_event event;
    event.events = events;
    event.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
}

void xps_loop_detach(int epoll_fd, int fd) {
    /* detach fd from epoll */
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void xps_loop_run(int epoll_fd) {
    while (1) {
        logger(LOG_DEBUG, "xps_loop_run()", "epoll wait");
        int n_ready_fds = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, -1);
        logger(LOG_DEBUG, "xps_loop_run()", "epoll wait over");

        // Process events
        for (int i = 0; i < n_ready_fds; i++) {
        int curr_fd = events[i].data.fd;

        // Checking if curr_fd is of a listener
        xps_listener_t *listener = NULL;
        for (int i = 0; i < listeners.length; i++) {
            xps_listener_t *curr = listeners.data[i];
            if (curr != NULL && curr->sock_fd == curr_fd) {
                listener = curr;
                break;
            }
        }
        if (listener) {
            xps_listener_connection_handler(listener);
            continue;
        }

        // Checking if curr_fd is of a connection
        xps_connection_t *connection = NULL;

        /* iterate through the connections and check if curr_fd is of a connection */
        for(int i = 0; i < connections.length; i++) {
            xps_connection_t *curr = connections.data[i];
            if (curr != NULL && curr->sock_fd == curr_fd) {
                connection = curr;
                break;
            }
        }

        if (connection)
            xps_connection_read_handler(connection);
        }
    }
}