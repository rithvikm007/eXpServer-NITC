#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>

#define PORT 8080
#define BUFF_SIZE 10000
#define MAX_ACCEPT_BACKLOG 5
#define MAX_EPOLL_EVENTS 10

// Function to reverse a string in-place
void strrev(char *str) {
    for (int start = 0, end = strlen(str) - 2; start < end; start++, end--) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
    }
}

int main() {
    // Creating listening sock
    int listen_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Setting sock opt reuse addr
    int enable = 1;
    setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    // Creating an object of struct socketaddr_in
    struct sockaddr_in server_addr;

    // Setting up server addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Binding listening sock to port
    bind(listen_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Starting to listen
    listen(listen_sock_fd, MAX_ACCEPT_BACKLOG);
    printf("[INFO] Server listening on port %d\n", PORT);

    int epoll_fd = epoll_create1(0);

    struct epoll_event event, events[MAX_EPOLL_EVENTS];

    event.events = EPOLLIN;
    event.data.fd = listen_sock_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock_fd, &event);

    while (1) {
        printf("[DEBUG] Epoll wait\n");
        int n_ready_fds = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, -1);

        for(int i = 0; i < n_ready_fds; i++){
            int curr_fd = events[i].data.fd;

            if(curr_fd == listen_sock_fd) {
                // Creating an object of struct socketaddr_in
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);;
                
                // Accept client connection
                int conn_sock_fd = accept(listen_sock_fd, (struct sockaddr *)&client_addr,
                &client_addr_len);

                if(conn_sock_fd < 0){
                    printf("[ERROR] Error occured: %s.\n", strerror(errno));
                    continue;
                }
                printf("[INFO] Client connected to server\n");

                event.events = EPOLLIN;
                event.data.fd = conn_sock_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock_fd, &event);
            }
            else {
                // Create buffer to store client message
                char buff[BUFF_SIZE];
                memset(buff, 0, BUFF_SIZE);
                // Read message from client to buffer
                ssize_t read_n = recv(curr_fd, buff, sizeof(buff), 0);
                
                // Client closed connection or error occurred
                if (read_n < 0) {
                    printf("[INFO] Error occured.\n");
                    close(curr_fd);
                    break;
                } else if (read_n == 0) {
                    printf("[INFO] Client Disconnected.\n");
                    close(curr_fd);
                    break;
                }
                
                // Print message from client
                printf("[CLIENT MESSAGE] %s", buff);
                
                // Sting reverse
                strrev(buff);
                
                // Sending reversed string to client
                send(curr_fd, buff, read_n, 0);
            }
        }
    }
}
