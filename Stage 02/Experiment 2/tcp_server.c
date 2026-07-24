#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFF_SIZE 10000
#define MAX_ACCEPT_BACKLOG 5

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

    while (1) {
        // Creating an object of struct socketaddr_in
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;
        
        // Accept client connection
        int conn_sock_fd = accept(listen_sock_fd, (struct sockaddr *)&client_addr,
        &client_addr_len);

        if(conn_sock_fd < 0){
			printf("[ERROR] Error occured.\n");
			continue;
		}
        printf("[INFO] Client connected to server\n");

        while (1) {
            // Create buffer to store client message
            char buff[BUFF_SIZE];
            memset(buff, 0, BUFF_SIZE);
            // Read message from client to buffer
            ssize_t read_n = recv(conn_sock_fd, buff, sizeof(buff), 0);
            
            // Client closed connection or error occurred
            if (read_n < 0) {
                printf("[INFO] Error occured.\n");
                close(conn_sock_fd);
                break;
            } else if (read_n == 0) {
                printf("[INFO] Client Disconnected.\n");
                close(conn_sock_fd);
                break;
            }
            
            // Print message from client
            printf("[CLIENT MESSAGE] %s", buff);
            
            // Sting reverse
            strrev(buff);
            
            // Sending reversed string to client
            send(conn_sock_fd, buff, read_n, 0);
        }
    }
}
