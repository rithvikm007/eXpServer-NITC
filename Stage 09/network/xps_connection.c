#include "../xps.h"

void connection_loop_read_handler(void *ptr);
void connection_loop_write_handler(void *ptr);
void connection_loop_close_handler(void *ptr);
void connection_read_handler(void *ptr);
void connection_write_handler(void *ptr);

xps_connection_t *xps_connection_create(xps_core_t *core, u_int sock_fd) {

    xps_connection_t *connection = malloc(sizeof(xps_connection_t));
    if (connection == NULL) {
        logger(LOG_ERROR, "xps_connection_create()", "malloc() failed for 'connection'");
        return NULL;
    }

    /* attach sock_fd to epoll */
    xps_loop_attach(core->loop, sock_fd, EPOLLIN | EPOLLOUT | EPOLLET, connection, connection_loop_read_handler, connection_loop_write_handler, connection_loop_close_handler);

    // Init values
    connection->core = core;
    connection->sock_fd = sock_fd;
    connection->listener = NULL;
    connection->remote_ip = get_remote_ip(sock_fd);
    connection->write_buff_list = xps_buffer_list_create();
    connection->read_ready = false;
    connection->write_ready = false;
    connection->send_handler = connection_write_handler;
    connection->recv_handler = connection_read_handler;

    /* add connection to 'connections' list */
    vec_push(&core->connections, connection);

    logger(LOG_DEBUG, "xps_connection_create()", "created connection");
    return connection;
}

void xps_connection_destroy(xps_connection_t *connection) {

    /* validate params */
    assert(connection != NULL);

    /* set connection to NULL in 'connections' list */
    xps_core_t *core = connection->core;
    for(int i = 0; i < core->connections.length; i++) {
        xps_connection_t *curr = core->connections.data[i];
        if (curr == connection) {
            core->connections.data[i] = NULL;
            break;
        }
    }

    /* detach connection from loop */
    xps_loop_detach(connection->core->loop, connection->sock_fd);

    /* close connection socket FD */
    close(connection->sock_fd);

    /* free connection->remote_ip */
    free(connection->remote_ip);

    /* free connection instance */
    free(connection);

    logger(LOG_DEBUG, "xps_connection_destroy()", "destroyed connection");
}

void strrev(char *str) {
    int len = strlen(str);
    int start = 0;
    int end = len - 1;
    
    // Check if last char is newline
    if (len > 0 && str[end] == '\n') {
        end--;  // Don't reverse the newline
    }

    while (start < end) {
        char temp = str[start];
        str[start++] = str[end];
        str[end--] = temp;
    }
}

void connection_read_handler(void *ptr) {
    xps_connection_t *connection = (xps_connection_t *)ptr;
    assert(connection != NULL);

    char buff[DEFAULT_BUFFER_SIZE];
    long read_n = recv(connection->sock_fd, buff, sizeof(buff) - 1, 0);

    if (read_n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            logger(LOG_DEBUG, "xps_connection_read_handler()", "recv() would block, try again later");
            connection->read_ready = false;
            return;
        }
        else {
            logger(LOG_ERROR, "xps_connection_read_handler()", "recv() failed");
            perror("Error message");
            xps_connection_destroy(connection);
            return;
        }
    }

    if (read_n == 0) {
        logger(LOG_INFO, "connection_read_handler()", "peer closed connection");
        xps_connection_destroy(connection);
        return;
    }

    buff[read_n] = '\0';

    /* print client message */
    printf("[CLIENT MESSAGE] %s\n", buff);

    /* reverse client message */
    strrev(buff);

    // Create a new buffer with the reversed message and append it to the write buffer list
    xps_buffer_t *buffer = xps_buffer_create(read_n, read_n, NULL);
    memcpy(buffer->data, buff, read_n);
    xps_buffer_list_append(connection->write_buff_list, buffer);
}

void connection_write_handler(void *ptr) {
    xps_connection_t *connection = (xps_connection_t *)ptr;
    assert(connection != NULL);

    if(!connection->write_buff_list || connection->write_buff_list->len == 0) {
        logger(LOG_DEBUG, "connection_loop_write_handler()", "no data to write");
        return;
    }

    xps_buffer_t *buffer = xps_buffer_list_read(connection->write_buff_list, connection->write_buff_list->len);

    if(buffer == NULL) {
        logger(LOG_ERROR, "connection_loop_write_handler()", "xps_buffer_list_read() failed");
        return;
    }

    long bytes_written = 0;
    long message_len = buffer->len;
    while(bytes_written < message_len) {
        long write_n = send(connection->sock_fd, buffer->data + bytes_written, message_len - bytes_written, 0);
        if (write_n < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                logger(LOG_DEBUG, "connection_loop_write_handler()", "send() would block, try again later");
                connection->write_ready = false;
                return;
            }
            else {
                logger(LOG_ERROR, "connection_loop_write_handler()", "send() failed");
                perror("Error message");
                xps_connection_destroy(connection);
                xps_buffer_destroy(buffer);
                return;
            }
        }
        bytes_written += write_n;
        xps_buffer_list_clear(connection->write_buff_list, write_n);
    }
    xps_buffer_destroy(buffer);
}

void connection_loop_close_handler(void *ptr) {
    xps_connection_t *connection = (xps_connection_t *)ptr;
    assert(connection != NULL);

    logger(LOG_INFO, "connection_loop_close_handler()", "connection closed");
    xps_connection_destroy(connection);
}

void connection_loop_read_handler(void* ptr) {
    assert(ptr != NULL);
    xps_connection_t *connection = (xps_connection_t *)ptr;
    connection->read_ready = true;
}

void connection_loop_write_handler(void* ptr) {
    assert(ptr != NULL);
    xps_connection_t *connection = (xps_connection_t *)ptr;
    connection->write_ready = true;
}